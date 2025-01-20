#!/bin/bash
sudo DEBIAN_FRONTEND=noninteractive apt-get install flex libnet1 libnet1-dev libpcap-dev -y
sudo cp -r /users/dbosch/share-education/SecuringLegacySystems_JHU/flooder /usr/local/src
cd /usr/local/src/flooder; sudo make; sudo cp flooder /bin

