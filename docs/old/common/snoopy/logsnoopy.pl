# No arguments needed, will get them from machine
open(HN,"hostname |");
while(<HN>)
{
    @items = split /\./, $_;
    $host=$items[0];
    $exp=$items[1];
    $proj=$items[2];
}
open(PR,"ls /proj |");
while(<PR>)
{
    $sysproj = $_;
    $sysproj =~ s/\s//;
    if (lc($sysproj) eq lc($proj))
    {
	$proj = $sysproj;
	last;
    }
}
open(EX,"ls /proj/$proj/exp |");
while(<EX>)
{
    $sysexp = $_;
    $sysexp =~ s/\s//;
    if (lc($sysexp) eq lc($exp))
    {
	$exp = $sysexp;
	last;
    }
}
print "$host $exp $proj\n";
$fn = "/proj/" . $proj . "/exp/" . $exp . "/logs/snoopy." . $host;
$time=time;
if (-e $fn)
{
    system("mv $fn $fn.$time");
}	
while(1)
{
    system("perl /usr/local/src/snoopy/cleanlog.pl > /proj/$proj/exp/$exp/logs/snoopy.$host");
    sleep(60);
}
