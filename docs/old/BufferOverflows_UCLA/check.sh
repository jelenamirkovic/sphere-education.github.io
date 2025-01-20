# Two arguments, experiment and project name
str=$0
CPATH=${str%/*}
CPATH="$CPATH/../common"
echo "Path $CPATH"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl elinks links wget curl tcpdump"
echo "Checking files"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /usr/src/fhttpd /usr/src/fhttpd/webserver /usr/src/fhttpd/index.html /usr/src/fhttpd/frobnick/*.frob.txt"

