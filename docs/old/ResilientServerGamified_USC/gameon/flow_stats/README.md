SENSS - flow stats
======================

flow_collection is very similar to tcpstat (www.frenchfries.net/paul/tcpstat/).
Both summarize flows (both ongoing and finished). This program gives CDF and info on
half-open connections and durations of finished connections.

In this directory is a python script using WAND's libtrace + WAND's python
bindings to analyze network traces for basic stats (which may lead to
identifying DoS attacks).

Compiling/installing/running
----------------------------
### Requirements
- [WAND's libtrace 3.0.22 (or higher)](http://research.wand.net.nz/software/libtrace.php)
- [WAND's libtrace python bindings](https://www.cs.auckland.ac.nz/~nevil/python-libtrace-1.6.tgz)
- [py27-ipaddress](https://pypi.python.org/pypi/ipaddress)
- [bitarray](https://pypi.python.org/pypi/bitarray/0.8.1)
- [numpy](http://www.numpy.org/)

### Installing 

Currently there is no setup.py or install script. 
To run, follow the links above and install required software, then run locally in directory.

### Running

python2.7 flow_collection.py {TRACEURI}

The trace URI takes WAND's URI format: FORMAT:FILENAME. Eg. int:eth0 or pcapfile:trace101.dump.

If no FORMAT is given, pcap is assumed.

### Options

usage: flow_collection.py [-h] [-i INTERVAL] [-d TARGET] [-b FILTER]
                          [--flowdb FLOW_DB_NAME] [--destdb DEST_DB_NAME] [-p]
                          input

Summerizes flows from a device or trace.

positional arguments:
  input                 Trace URI (eg. 'int:eth0' or 'pcapfile:./trace.dump')
                        If no type given, assumed to be pcapfile.

optional arguments:
  -h, --help            show this help message and exit
  -i INTERVAL, --interval INTERVAL
                        Interval in seconds to print/calculate stats over
                        (seconds based on input Trace URI).
  -d TARGET, --target TARGET
                        Focus on a specific target IP or target net.
  -b FILTER, --bpf FILTER
                        BPF style filter to apply.
  --flowdb FLOW_DB_NAME, --fdb FLOW_DB_NAME
                        Name of database file for flow info. Default
                        '/tmp/flowdb'
  --destdb DEST_DB_NAME, --ddb DEST_DB_NAME
                        Name of database file for destination summary info.
                        Default '/tmp/destdb'
  -p, --keep_dbs        Keep databases around after program finishes.
Authors
----------------------
Genevieve Bartlett bartlett@isi.edu Oct. 2015

Bugs
----------------------------
- Should eventually be threaded and optimized. 
  Right now under large load (eg. traces/input with ~.5Gbps or greater bw to the destinations of interest) 
  the program may drop packets while updating tables. 

- Timing may be an issue (timing to update tables, to report stats and to keep up with live input). 
 
Please report any bugs found to bartlett@isi.edu

