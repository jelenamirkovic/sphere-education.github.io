#!/bin/bash

i="1"
DATE=`date +%s`
TMP="/tmp/submit$i-$DATE"

echo "This script will create an archive named:"
echo ""
echo "$USER-submit$i-$DATE.tar.gz" in the current directory.
echo ""
echo "This file will contain:"
echo "	* /usr/src/fhttpd"
echo "	* /root/part1"
echo "	* /root/exploit$i.sh"
echo "	* /root/payload$i"
echo ""
echo "The script restore$i.sh will restore these files."

sudo mkdir $TMP
sudo cp -ar /usr/src/fhttpd $TMP
sudo cp -ar /root/part$i $TMP
sudo cp -a /root/exploit$i.sh $TMP
sudo cp -a /root/payload$i $TMP

sudo tar -czf $USER-exploits$i-$DATE.tar.gz $TMP
