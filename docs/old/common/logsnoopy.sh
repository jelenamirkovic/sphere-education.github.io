sudo apt-get install -y socat
cp -r /share/education/common/snoopy /usr/local/src/snoopy
cp /share/education/common/snoopy/logsnoopy.pl /usr/local/src/snoopy/logsnoopy.pl
cd /usr/local/src/snoopy
sudo ./configure
sudo make
sudo make install
sudo make enable
sudo perl /usr/local/src/snoopy/start.pl
sudo reboot
