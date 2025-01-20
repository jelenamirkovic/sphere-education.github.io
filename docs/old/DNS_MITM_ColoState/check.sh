# Two arguments, experiment and project name
str=$0
CPATH=${str%/*}
CPATH="$CPATH/../common"
echo "Path $CPATH"
echo "=====> Checking auth"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no auth.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl bind9 dnsutils dnssec-tools"
echo "Checking files"
ssh -o stricthostkeychecking=no auth.$1.$2 "cd $CPATH; sudo /usr/bin/perl check-files.pl /etc/bind/google.com /etc/bind/10"
echo "Checking processes"
ssh -o stricthostkeychecking=no auth.$1.$2 "cd $CPATH; /usr/bin/perl check-processes.pl named"
echo "=====> Checking cache"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no cache.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl bind9 dnsutils"
echo "=====> Checking attacker"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no attacker.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl ettercap tshark"
cmd="dig www.google.com A"
out="10.1.2.155"
ssh client.$1.$2 "cd $CPATH; perl check-output.pl \"${cmd}\" \"${out}\""


