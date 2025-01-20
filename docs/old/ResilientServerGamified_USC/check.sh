# Two arguments, experiment and project name
CPATH=`pwd`
CPATH="$CPATH/../common"
echo "Path $CPATH"
echo "Checking files"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /var/www/index.html"
echo "Expect only one of the next three checks to show flooder and score"
ssh -o stricthostkeychecking=no client1.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /usr/local/src/flooder /var/log/score"
ssh -o stricthostkeychecking=no client2.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /usr/local/src/flooder /var/log/score"
ssh -o stricthostkeychecking=no client3.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /usr/local/src/flooder /var/log/score"
echo "Checking processes"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-processes.pl nweb"
ssh -o stricthostkeychecking=no client1.$1.$2 "cd $CPATH; /usr/bin/perl check-processes.pl install-client"
ssh -o stricthostkeychecking=no client2.$1.$2 "cd $CPATH; /usr/bin/perl check-processes.pl install-client"
ssh -o stricthostkeychecking=no client3.$1.$2 "cd $CPATH; /usr/bin/perl check-processes.pl install-client"


