#!/bin/bash

DATE=`date +%s`
TMP="/tmp/submit-$DATE"

echo "This script will create an archive named:"
echo ""
echo "$USER-submit-$DATE.tar.gz" in the current directory.
echo ""
echo "This file will contain:"
echo "	* /var/www/html/FCCU.php"
echo " ... and the contents of:"
echo "	* /root/submission"
echo " ... which should include your exploit.txt and FCCU.patch"
echo ""
echo "The script restore.sh will restore these files."

sudo mkdir $TMP
sudo cp -a /usr/lib/cgi-bin/FCCU.php $TMP
sudo cp -ar /root/submission $TMP

sudo tar -czf $USER-sql-$DATE.tar.gz $TMP
