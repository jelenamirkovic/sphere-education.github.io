# Two arguments, experiment and project name

str=$0
CPATH=${str%/*}
CPATH="$CPATH/../common"
echo "Path $CPATH"
echo "Checking files"
ssh -o stricthostkeychecking=no intro.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /usr/share/pixmaps/*INTRO* /dev/*intro*  /var/log/*intro*  /etc/*intro* /.hidden/*Intro*"
