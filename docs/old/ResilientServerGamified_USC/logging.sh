#!/bin/bash
sudo apparmor_parser -R /etc/apparmor.d/usr.sbin.tcpdump
dev=`ip route get 10.1.5.2 | awk '{print $5}'|head -1`
opid=0
while :
  do
     perl /share/education/ResilientServerGamified_USC/score.pl /var/log/cctf.log > /var/log/score
     tcpdump -i $dev -w /var/log/cctf.log -U & 
     pid=$!
     if [ $opid -ne 0 ] ; then
	 echo "Killing $opid"
	 kill -9 $opid
     fi
     opid=$pid 
     sleep 10
  done

