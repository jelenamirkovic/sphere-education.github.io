#!/bin/bash

sudo echo "nameserver 10.1.1.2" > /etc/resolvconf/resolv.conf.d/head
sudo resolvconf -u
eth=`sudo ip route get 2.2.2.5 | awk '{print $3}'`
sudo ip route add 2.0.0.0/8 via $eth
