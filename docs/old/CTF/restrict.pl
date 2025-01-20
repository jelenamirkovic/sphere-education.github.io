#!/usr/bin/perl

system("chmod u+w /usr/local/etc/emulab/rc/rc.testbed");
$line = "/usr/bin/python /share/education/CTF/remove_group.py ";
for($i=0; $i<=$#ARGV; $i++)
{
    $line = $line . " " . $ARGV[$i];
}
system($line);
$found = 0;
$fh = new IO::File("/usr/local/etc/emulab/rc/rc.testbed");
while(<$fh>)
{
    if ($_ =~ /remove_group/)
    {
	$found = 1;
    }
}
close($fh);
if (!$found)
{
    open(my $fh, '>>', '/usr/local/etc/emulab/rc/rc.testbed');
    print $fh "$line\n";
}

