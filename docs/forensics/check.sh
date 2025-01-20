CPATH="/share/education/common"
echo "Path $CPATH"
cd $CPATH; scp -r * forensics:/tmp
echo "Checking installed programs"
ssh -o stricthostkeychecking=no forensics "cd /tmp; /usr/bin/perl check-installed.pl hexedit john"
echo "Checking files"
ssh -o stricthostkeychecking=no forensics "cd /tmp; /usr/bin/perl check-files.pl /usr/bin/e2undel /images/* /usr/bin/zonk"
