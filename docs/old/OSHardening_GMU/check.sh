# Two arguments, experiment and project name
str=$0
CPATH=${str%/*}
CPATH="$CPATH/../common"
echo "Path $CPATH"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no nodeA.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl php apache2 mysql bind9 vsftpd telnetd"
echo "Checking processes"
ssh -o stricthostkeychecking=no nodeA.$1.$2 "cd $CPATH; /usr/bin/perl check-processes.pl vsftpd apache2 mysql named inetd"

