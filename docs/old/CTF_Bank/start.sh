#!/bin/bash
HOST=`/bin/hostname`;
BLUE="^blue";
BLUEGW="^bluegateway";
REDGW="^redgateway";
if [[ $HOST =~ $BLUE ]] ; then
    if [[ $HOST =~ $BLUEGW ]] ; then
	sudo ip route add 2.0.0.0/8 via 10.1.1.3
    else
	sudo ip route add 2.0.0.0/8 via 1.1.1.2
    fi
else
    if [[ $HOST =~ $REDGW ]] ; then
	sudo ip route add 2.0.0.0/8 via 2.2.2.2
    else
	sudo ip route add 2.0.0.0/8 via 10.1.1.3
    fi
    /share/education/CTF_Bank/fixresolve.sh
fi
