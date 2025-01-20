# Two arguments, experiment and project name
str=$0
CPATH=${str%/*}
CPATH="$CPATH/../common"
echo "===> Checking client"
echo "Path $CPATH"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no client.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl nmap elinks links wget curl tcpdump"
echo "===> Checking server"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl apache2"
echo "Checking files"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /root/submit.sh /root/restore.sh /etc/mysql/my.cnf"
echo "Checking processes"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-processes.pl apache2 mysql"

