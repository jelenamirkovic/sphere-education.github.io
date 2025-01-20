# Two arguments, experiment and project name
str=$0
CPATH=${str%/*}
CPATH="$CPATH/../common"
echo "Path $CPATH"
echo "Checking installed programs"
echo "server"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl icedtea traceroute"
echo "Checking files"
echo "snort"
ssh -o stricthostkeychecking=no snort.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /usr/local/bin/snort"
echo "client1"
ssh -o stricthostkeychecking=no client1.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /bin/flooder /home/test/*"
for host in "server" "client2" "outsider" "internal" ; do
    echo $host
    ssh -o stricthostkeychecking=no $host.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /home/test/*"
done

