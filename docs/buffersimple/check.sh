CPATH="/share/education/common"
echo "Path $CPATH"
echo "Checking installed programs"
cd $CPATH; scp -r * bufferoverflow:/tmp
ssh -o stricthostkeychecking=no bufferoverflow "cd /tmp; /usr/bin/perl check-installed.pl elinks links wget curl tcpdump"
echo "Checking files"
ssh -o stricthostkeychecking=no bufferoverflow "cd /tmp; /usr/bin/perl check-files.pl /usr/src/fhttpd /usr/src/fhttpd/webserver /usr/src/fhttpd/index.html /usr/src/fhttpd/frobnick/*.frob.txt"