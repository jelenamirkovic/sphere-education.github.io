# Two arguments, experiment and project name
CPATH="/share/education/common"
echo "Path $CPATH"
ARRAY=(  asn1 asn2 asn3 asn4 server client attacker )
for node in ${ARRAY[*]} ; do
    echo $node
    cd $CPATH; scp -r * $node:/tmp
    echo "Checking installed programs"
    ssh -o stricthostkeychecking=no $node "cd /tmp; /usr/bin/perl check-installed.pl quagga traceroute"
   echo "Checking processes"
   ssh -o stricthostkeychecking=no $node "cd /tmp; /usr/bin/perl check-processes.pl quagga"
   if [ "$node" == "attacker"  -o  "$node" == "server" ] ; then
       echo "Checking installed programs"
       ssh -o stricthostkeychecking=no $node "cd /tmp; /usr/bin/perl check-installed.pl vsftpd"
       echo "Checking processes"
       ssh -o stricthostkeychecking=no $node "cd /tmp; /usr/bin/perl check-processes.pl vsftpd"
   fi
done
