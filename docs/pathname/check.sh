#!/usr/bin/env bash
CPATH="/share/education/common"
echo "Path $CPATH"
cd $CPATH; scp -r * pathname:/tmp
echo "Checking installed programs"
ssh -o stricthostkeychecking=no pathname "cd /tmp; /usr/bin/perl check-installed.pl php apache2 mysql perl elinks links wget curl tcpdump hexedit"
echo "Checking files"
ssh -o stricthostkeychecking=no pathname "cd /tmp; /usr/bin/perl check-files.pl /home/wilbar /home/megaboz /home/barbazzo /home/gustar /root/memo /usr/lib/cgi-bin/memo.cgi /usr/lib/cgi-bin/demo.cgi /usr/lib/cgi-bin/memo.pl /var/www/html/index.html /root/submit.sh /root/restore.sh"
echo "Checking processes"
ssh -o stricthostkeychecking=no pathname "cd /tmp; /usr/bin/perl check-processes.pl apache2"
echo "Checking cgi"
cmd="curl -s pathname/cgi-bin/memo.cgi"
perl ../common/check-output.pl "${cmd}" "^<html><head>"
cmd="curl -s pathname/cgi-bin/demo.cgi"
perl ../common/check-output.pl "${cmd}" "^<html><head>"

# # ensure paths set back to production values
# if ! grep 'tb-set-node-startcmd $server "sudo /share/education' pathname.ns &> /dev/null
# then
# 	echo "Ensure that pathname.ns refers to production location in /share/education!"
# fi

# # if ! grep 'export BASE="/share/education"' install &> /dev/null
# then
# 	echo "Ensure that install refers to production location in /share/education!"
# fi

