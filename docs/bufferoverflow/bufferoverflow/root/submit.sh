#!/bin/bash

DATE=`date +%s`
TMP="/tmp/submit-$DATE"

echo "This script will create an archive named:"
echo ""
echo "$USER-submit-$DATE.tar.gz" in the current directory.
echo ""
echo "This file will contain:"
echo "	* /usr/src/fhttpd"
echo "	* /root/submission (and everything in it)"
echo ""
echo "The script restore.sh will restore these files."

sudo mkdir $TMP
sudo cp -ar /usr/src/fhttpd $TMP
sudo cp -ar /root/submission $TMP

sudo tar -czf $USER-exploits-$DATE.tar.gz $TMP
