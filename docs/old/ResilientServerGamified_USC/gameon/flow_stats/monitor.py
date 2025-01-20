
import signal
import sys
sys.path.insert(0, "/usr/local/lib/");

import plt
import argparse
import ipaddress
from storage import *

TCP=0
UDP=1
ICMP=2
  
def deal_with_arguments():
  parser = argparse.ArgumentParser(description='Summarizes flows from a device or trace.')
  parser.add_argument('-i', '--interval', dest='interval', default=60, type=int, help='Interval in seconds to print/calculate stats over (seconds based on input Trace URI).')
  parser.add_argument('-d', '--target', dest='target', default=None, help='Focus on a specific target IP or target net.')
  parser.add_argument('-b', '--bpf', dest='filter', default=None, help="BPF style filter to apply.")
  parser.add_argument('--db', '--db', dest='db_name', default='/tmp/baseline.fs', help="Name of database file for flow info. Default '/tmp/baseline'")
  parser.add_argument('input', nargs=1, help="Trace URI (eg. 'int:eth0' or 'pcapfile:./trace.dump') If no type given, assumed to be pcapfile.")

  args = parser.parse_args()
  if ':' not in  args.input[0]:
    print("No type for input given. Assuming pcap file.")
    args.input = 'pcapfile:' + args.input[0]
  else:
    args.input = args.input[0]
  return args

def signal_handler(signal, frame):
  sys.exit(0)


def main():
  signal.signal(signal.SIGINT, signal_handler)
  args = deal_with_arguments()
  
  # Set up our storage.
  try:
    outputfile = open(args.db_name, 'w')
    tcp_flows = FlowStorage(filename=args.db_name)
    udp_flows = FlowStorage(filename=args.db_name)
    icmp_flows = FlowStorage(filename=args.db_name)
    dests = DestStorage(filename=args.db_name)
  except Exception as e:
    print("Problem setting up databases:\n\t%s" % e)  
    exit()

  # Try opening our trace.
  try:
    t = plt.trace(args.input)
  except Exception as e:
    print("Trouble opening trace URI/device:\n\t%s" % e)
    exit()
    
  # Try setting up our filter if given one.
  try:
    if args.filter != None or args.target != None:
      if args.filter != None and args.target != None:
          args.filter = args.filter + " and "
      elif args.filter == None:
        args.filter = ""
      if args.target != None:
        args.filter = args.filter + "dst "
        if '/' in args.target:
          args.filter = args.filter + "net "
        args.filter = args.filter + args.target
      f = plt.filter(args.filter)
      print("Applying filter \"%s\"" % args.filter)
      t.conf_filter(f)
  except Exception as e:
    print("Trouble applying bpf filter: \'%s\'\n\t%s"% (args.filter,e))
    exit()
    
  print("Press Ctrl+C to exit.")

  # Try reading.
  try:
    t.start()
  except Exception as e:
    print(e)
    exit()

  start_ts = 0
  # And we're off - loop for as long as we get packets (or ^C)
  packet_count = 0
  non_ipv4_packets = 0
  non_ip_packets = 0
  packets = dict()
  Bytes = dict()
  for proto in (TCP, UDP, ICMP):
    packets[proto] = 0
    Bytes[proto] = 0
  last_time_check_pkt_ts = 0
  current_time = int(time.time())
  last_packet_count = 0
  last_ts = -2
  current_ts = -1

  print("Packets processed:\t"),
  for pkt in t:
    if packet_count == 0:
      start_ts = pkt.seconds
    if pkt.seconds - start_ts > args.interval:
      break
    packet_count = packet_count + 1
    last_packet_count = last_packet_count + 1
    last_ts = current_ts
    current_ts = pkt.seconds
    
    if last_ts > (current_ts + .5):
      print("PROBLEM: timestamps out of order: Last ts: %f this ts: %f" %(last_ts, current_ts))
      exit()
    
    print("%012d\b\b\b\b\b\b\b\b\b\b\b\b" % packet_count),
    
    # If we've gone through an interval's worth of packets, check the actual time
    # we may be going through packets *much* faster than real-time if we're
    # reading from a trace, but this keeps us from constantly checking the time
    # to determine when we update/print stats.
    if current_ts - last_time_check_pkt_ts > 3 or last_packet_count > 100:
      current_time = int(time.time())
      last_time_check_pkt_ts = current_ts 
        
    # IP layer
    ip = pkt.ip    
    if not ip:
      non_ip_packets = non_ip_packets+1 
      continue
    
    numbytes = ip.pkt_len
    # IPv4
    if ip.version != 4:
      non_ipv4_packets = non_ipv4_packets + 1
      continue
    
    # TCP is al we care about right now.
    tcp = pkt.tcp
    udp = pkt.udp
    icmp = pkt.icmp
    if pkt.tcp:
      proto = TCP
    elif pkt.udp:
      proto = UDP
    elif pkt.icmp:
      proto = ICMP
    else:
      continue
    packets[proto] = packets[proto]+1
    Bytes[proto] = Bytes[proto] + numbytes

    if proto == ICMP:
      icmp_flows.add_flow_event(current_ts, pkt_src, 0, pkt_dst, 0, OTHER, numbytes)
    if proto == TCP or proto == UDP:
      # Get what we're using for dictionary keys.
      pkt_src = str(ip.src_prefix)
      pkt_dst = str(ip.dst_prefix)
      if proto == TCP:
        pkt_sport = str(tcp.src_port)
        pkt_dport = str(tcp.dst_port)
      else:
        pkt_sport = str(udp.src_port)
        pkt_dport = str(udp.dst_port)
      
    # Not perfect - we don't check for certain combinations (ie combinations that don't make sense).
      event = OTHER
      if tcp:
        if tcp.syn_flag:
          if not tcp.ack_flag:
            event = SYN
          else:
            event = SYN_ACK
        elif tcp.fin_flag:
          event = FIN
        elif tcp.rst_flag:
          event = RST
        elif tcp.ack_flag or tcp.urg_flag or tcp.psh_flag:
          event=ACK_AND_DATA
        else:
          event=OTHER
      
      # Ready to store.
      if tcp:
        tcp_flows.add_flow_event(current_ts, pkt_src, pkt_sport, pkt_dst, pkt_dport, event, numbytes)
      else:
        udp_flows.add_flow_event(current_ts, pkt_src, pkt_sport, pkt_dst, pkt_dport, event, numbytes)
      
  
  print("\n************ OVERALL STATS ******************\n")
  print("TCP packets\t%s\nUDP packets\t%s\nICMP packets\t%s\n" % (packets[TCP], packets[UDP], packets[ICMP]))
  print >>outputfile, "ALL %s %s %s %s %s %s" % (packets[TCP], packets[UDP], packets[ICMP], Bytes[TCP], Bytes[UDP], Bytes[ICMP])
  dests.print_stats(tcp_flows, 'TCP', outputfile)
  dests.print_stats(udp_flows, 'UDP', outputfile)
  dests.print_stats(icmp_flows, 'ICMP', outputfile)

if __name__ == "__main__":
  main()
  dests = Destination()  
