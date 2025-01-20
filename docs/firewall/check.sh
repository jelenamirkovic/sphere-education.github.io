CPATH="/share/education/common"

echo "===> Checking client"
echo "Path $CPATH"
echo "Copying files into server node."
cd $CPATH; scp -r * server:/tmp
echo "Copying files into client node."
cd $CPATH; scp -r * client:/tmp
echo "Checking installed programs"
ssh -o stricthostkeychecking=no client "cd /tmp; /usr/bin/perl check-installed.pl nmap elinks links wget curl tcpdump"
echo "===> Checking server"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no server "cd /tmp; /usr/bin/perl check-installed.pl apache2"
echo "Checking files"
ssh -o stricthostkeychecking=no server "cd /tmp; /usr/bin/perl check-files.pl /root/submit.sh /root/restore.sh /etc/mysql/my.cnf"
echo "Checking processes"
ssh -o stricthostkeychecking=no server "cd /tmp; /usr/bin/perl check-processes.pl apache2 mysql"
