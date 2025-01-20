# Two arguments, experiment and project name
str=$0
CPATH=${str%/*}
CPATH="$CPATH/../common"
echo "Path $CPATH"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl php apache2 mysql perl elinks links wget curl tcpdump"
echo "Checking files"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /var/www/html/fccu /usr/lib/cgi-bin /usr/lib/cgi-bin/FCCU.php /var/www/html/index.html /var/www/html/fccu/index.html /root/submit.sh /root/restore.sh"
echo "Checking processes"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-processes.pl apache2"
echo "Checking cgi"
cmd="curl -s server.$1.$2/cgi-bin/FCCU.php"
perl ../common/check-output.pl "${cmd}" "^<html><head><title>FrobozzCo Community Credit Union"

