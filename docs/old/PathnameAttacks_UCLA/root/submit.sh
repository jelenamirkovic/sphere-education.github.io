#!/bin/bash

DATE=`date +%s`
TMP="/tmp/submit-$DATE"
OUTFILE="$USER-pathname-$DATE.tar.gz"

echo "This script will create an archive named:"
echo ""
echo "'$OUTFILE' in the current directory."
echo ""
echo "This file will contain:"
echo "	* /usr/lib/cgi-bin/memo.pl"
echo "	* /root/submission"
echo "   ... which should contain:"
echo "	      exploit.sh"
echo "	      your memo"
echo "	      your patch"
echo ""
echo "The script restore.sh will restore these files."

sudo mkdir $TMP
sudo cp -a /usr/lib/cgi-bin/memo.pl $TMP
sudo cp -ar /root/submission $TMP

sudo tar -czf $OUTFILE $TMP
