#!/usr/bin/env bash
# Two arguments, experiment and project name
if [[ -z $1 || -z $2 ]]
then
	echo "./check.sh EXPNAME PROJNAME"
	exit 1
fi
str=$0
CPATH=${str%/*}
CPATH="$CPATH/../common"
echo "Path $CPATH"
echo "Checking installed programs"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-installed.pl php apache2 mysql perl elinks links wget curl tcpdump hexedit"
echo "Checking files"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-files.pl /home/wilbar /home/megaboz /home/barbazzo /home/gustar /root/memo /usr/lib/cgi-bin/memo.cgi /usr/lib/cgi-bin/demo.cgi /usr/lib/cgi-bin/memo.pl /var/www/html/index.html /root/submit.sh /root/restore.sh"
echo "Checking processes"
ssh -o stricthostkeychecking=no server.$1.$2 "cd $CPATH; /usr/bin/perl check-processes.pl apache2"
echo "Checking cgi"
cmd="curl -s server.$1.$2/cgi-bin/memo.cgi"
perl ../common/check-output.pl "${cmd}" "^<html><head>"
cmd="curl -s server.$1.$2/cgi-bin/demo.cgi"
perl ../common/check-output.pl "${cmd}" "^<html><head>"

# ensure paths set back to production values
if ! grep 'tb-set-node-startcmd $server "sudo /share/education' pathname.ns &> /dev/null
then
	echo "Ensure that pathname.ns refers to production location in /share/education!"
fi

if ! grep 'export BASE="/share/education"' install &> /dev/null
then
	echo "Ensure that install refers to production location in /share/education!"
fi

