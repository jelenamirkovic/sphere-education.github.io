#!/usr/bin/perl

if ($#ARGV < 0)
{
    print "Must supply file name for the tcpdump file\n";
    exit 1;
}
open(TC,"tcpdump -r $ARGV[0] -nn -tt |") || die "Failed: $!\n";
$cnt = 0;
$start = 0;
$time = 0;
while ( <TC> )
{
    @items = split /\s/, $_;
    $time = $items[0];
    if ($start == 0 || $time > $start + 1)
    {
	print "$time $cnt\n";
	$start = $time;
	$cnt = 0;
    }
    $cnt++;
}
print "$time $cnt\n";

	
