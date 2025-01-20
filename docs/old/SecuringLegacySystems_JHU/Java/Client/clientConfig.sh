#!/bin/bash
# if running Ubuntu
sudo apt-get update --fix-missing
sudo apt-get install icedtea-netx-common icedtea-netx traceroute -y
# if running Fedora
#sudo yum install java -y
sudo rm /home/test -r
sudo cp /share/education/SecuringLegacySystems_JHU/Java/Client /home/test -r
sudo chmod 777 /home/test
sleep 10
bash /home/test/runClient.sh
