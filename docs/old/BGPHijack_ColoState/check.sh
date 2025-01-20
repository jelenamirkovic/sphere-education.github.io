# Two arguments, experiment and project name
str=$0
CPATH=${str%/*}
CPATH="$CPATH/../common"
echo "Path $CPATH"
ARRAY=(  asn1 asn2 asn3 asn4 server client attacker )
for node in ${ARRAY[*]} ; do
   echo $node
   echo "Checking installed programs"
   ssh -o stricthostkeychecking=no $node.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl quagga traceroute"
   echo "Checking processes"
   ssh -o stricthostkeychecking=no $node.$1.$2 "cd $CPATH; /usr/bin/perl check-processes.pl quagga"
   if [ "$node" == "attacker"  -o  "$node" == "server" ] ; then
       echo "Checking installed programs"
       ssh -o stricthostkeychecking=no $node.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl vsftpd"
       echo "Checking processes"
       ssh -o stricthostkeychecking=no $node.$1.$2 "cd $CPATH; /usr/bin/perl check-processes.pl vsftpd"
   fi
done
