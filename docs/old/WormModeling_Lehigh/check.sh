# Two arguments, experiment and project name
str=$0
CPATH=${str%/*}
CPATH="$CPATH/../common"
echo "Path $CPATH"
ssh -o stricthostkeychecking=no node-0.$1.$2 "bash /share/education/WormModeling_Lehigh/setup_nodes"
ARRAY=( node-0 node-1 node-2 node-3 node-4 node-5 node-6 node-7 node-8 )
for node in ${ARRAY[*]} ; do
   echo $node
   ssh -o stricthostkeychecking=no $node.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /mnt/local/paws_RT.dat"
done
ssh -o stricthostkeychecking=no node-0.$1.$2 "cd $MYPATH; bash make.sh; cd $CPATH; /usr/bin/perl check-files.pl /users/$USER/paws/log1.dat /users/$USER/paws/log2.dat /users/$USER/paws/log3.dat /users/$USER/paws/log4.dat /users/$USER/paws/log5.dat /users/$USER/paws/log6.dat /users/$USER/paws/log7.dat /users/$USER/paws/log8.dat"
