# Two arguments, experiment and project name
str=$0
CPATH=${str%/*}
CPATH="$CPATH/../common"
echo "Path $CPATH"
echo "Node 1"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no node1.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl ettercap tshark arping"
echo "Checking files"
ssh -o stricthostkeychecking=no node1.$1.$2 "cd $CPATH; sudo /usr/bin/perl check-files.pl /root/laughingskull.filter  /root/make-realmappings"
echo "Node 2"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no node2.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl apache2"
echo "Checking files"
ssh -o stricthostkeychecking=no node2.$1.$2 "cd $CPATH; sudo /usr/bin/perl check-files.pl /var/www/html/laughingskull.gif"
echo "Node 3"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no node3.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl lynx"
echo "Node 4"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no node4.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl apache2 vsftpd"
echo "Checking files"
ssh -o stricthostkeychecking=no node4.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /var/www/html/congratulations.jpg /var/www/html/index.html"



