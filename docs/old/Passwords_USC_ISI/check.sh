# Two arguments, experiment and project name
str=$0
CPATH=${str%/*}
CPATH="$CPATH/../common"
echo "Path $CPATH"
ssh -o stricthostkeychecking=no pass.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl john"
ssh -o stricthostkeychecking=no pass.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /var/passwords"
