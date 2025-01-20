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
	crontab /share/education/SecureServer_USC/bd.cron
    fi
else
    if [[ $HOST =~ $REDGW ]] ; then
	sudo ip route add 2.0.0.0/8 via 2.2.2.2
    else
	sudo ip route add 2.0.0.0/8 via 10.1.1.3
    fi
    /bin/bash /share/education/SecureServer_USC/fixresolve.sh
fi
