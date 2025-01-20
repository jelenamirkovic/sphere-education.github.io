# Two arguments, experiment and project name
str=$0
CPATH=${str%/*}
CPATH="$CPATH/../common"
echo "Path $CPATH"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no SWworkstation1.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl apache2"
ssh -o stricthostkeychecking=no NWworkstation1.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl lynx"
echo "Checking processes"
ssh -o stricthostkeychecking=no SWworkstation1.$1.$2 "cd $CPATH; /usr/bin/perl check-processes.pl apache2"
ARRAY=( NWrouter NWworkstation1 SWrouter SWworkstation1 ISrouter )
for node in ${ARRAY[*]} ; do
   echo "=========> $node"
   ssh -o stricthostkeychecking=no $node.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /tmp/nicaddressing.dat "
done
