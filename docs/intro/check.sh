# Two arguments, experiment and project name
CPATH="/share/education/common"
echo "Path $CPATH"
echo "Checking files"
cd $CPATH; scp -r * intro:/tmp
ssh -o stricthostkeychecking=no intro "cd /tmp; /usr/bin/perl check-files.pl /usr/share/pixmaps/*INTRO* /dev/*intro*  /var/log/*intro*  /etc/*intro* /.hidden/*Intro*"
