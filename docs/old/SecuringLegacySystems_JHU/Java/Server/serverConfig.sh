#!/bin/bash
sudo apt-get update --fix-missing
sudo apt-get install icedtea-netx-common icedtea-netx traceroute -y
sudo rm /home/test -r
sudo cp /share/education/SecuringLegacySystems_JHU/Java/Server /home/test -r
sleep 5
bash /home/test/server.sh
