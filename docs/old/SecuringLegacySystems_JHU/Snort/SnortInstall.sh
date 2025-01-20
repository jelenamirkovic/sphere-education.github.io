#!/bin/bash
sudo cp /share/education/SecuringLegacySystems_JHU/Snort/iptablesload /etc/network/if-pre-up.d/
sudo chmod +x /etc/network/if-pre-up.d/iptablesload
sudo iptables -P FORWARD DROP

sudo apt-get install flex bison libpcre3-dev libnetfilter-queue-dev libnetfilter-queue1 -y
cd /usr/local
sudo tar -xvzf /share/education/SecuringLegacySystems_JHU/Snort/libpcap-1.0.0.tar.gz
cd libpcap-1.0.0
sudo ./configure && sudo make && sudo make install
cd ..
sudo tar -zxvf /share/education/SecuringLegacySystems_JHU/Snort/libdnet-1.11.tar.gz
cd libdnet-1.11
sudo ./configure && sudo make && sudo make install
cd ..
sudo tar -xvzf /share/education/SecuringLegacySystems_JHU/Snort/daq-0.6.2.tar.gz
cd daq-0.6.2
sudo ./configure && sudo make && sudo make install
cd ..

sudo cp /usr/local/lib/libdnet.1* /usr/lib/ -r
sudo ldconfig

sudo tar -zxvf /share/education/SecuringLegacySystems_JHU/Snort/snort-2.9.2.2.tar.gz
cd snort-2.9.2.2
sudo ./configure --enable-targetbased --enable-dynamicplugin --enable-perfprofiling --enable-react && sudo make && sudo make install

sudo iptables -F FORWARD
sudo iptables -A FORWARD -j NFQUEUE
