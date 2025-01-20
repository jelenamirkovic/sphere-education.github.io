# Two arguments, experiment and project name
CPATH=`pwd`
CPATH="$CPATH/../common"
echo "Path $CPATH"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl php apache2 mysql perl"
echo "Checking files"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /var/www/html/index.php /var/www/html/process.php"
echo "Checking processes"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-processes.pl apache2"
echo "Checking php"
cmd="curl -s server.$1.$2/index.php"
perl ../common/check-output.pl "${cmd}" "Balance and transfer history"

