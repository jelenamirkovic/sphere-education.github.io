#! /usr/bin/perl

$|=1;
use Class::Struct;
use Math::BigInt;
use Sys::Hostname;

my $usage = "Usage: $0 tcpdump_file_location\n";
my $PRINTINT = 60;
my $fh;

struct( conn =>
{ fin => int, stime => double, etime => double});

my %hash = ();

my $red=0;
my $blue=0;
my $out=0;

# Main program
if ($#ARGV < 0) 
{
    die "$usage";
}
else
{
    my $file = $ARGV[0];
    my $start = 0;
    my $time = 0;
    my $stattime = 0;
    my $lastsyn = 0;
    my $lasttime = 0;

    open(FH,"tcpdump -nn -r $ARGV[0] -tt |") || die "tcpdump failed\n";
    while (<FH>) {
	my @items = split(/\s+/, $_);
	my $flags = $items[6];
        # This is a syn or a fin
	if ($flags =~ /S/ || $flags =~ /F/ || $flags =~ /R/)
	{
	    if ($start == 0)
	    {
		$start = $time;
	    }
	  
	    $items[4] =~ s/\://;
	    $time = $items[0];
	    $lasttime = $time;
	    my $int = $time - $stattime;
	    my $fromstart = $time - $start;
	    if ($int > $PRINTINT)
	    {
		$stattime = $time;
		for $cid (keys %hash)
		{
		    my $dur = $time - $hash{$cid}->{stime};
		    if ($dur < 0.5)
		    {
			
		    }
		    else
		    {
			$red ++;
			delete($hash{$cid});
		    }
		}
	    }
	    my $cid = $items[2] . " " . $items[4];
	    my $rid = $items[4] . " " . $items[2];
	    if (!exists($hash{$cid}) && !exists($hash{$rid}) && $flags =~ m/S/ && $time - $lastsyn >= 1)
	    {
		$hash{$cid} = new conn();
		$hash{$cid}->{stime} = $time;
		$hash{$cid}->{etime} = 0;
		$lastsyn = $time;
	    }
	    else
	    {
		my $fid="";
		if (exists($hash{$cid}))
		{
		    $fid=$cid;
		}
		elsif(exists($hash{$rid}))
		{
		    $fid=$rid;
		}
		if ($fid && ($flags =~ m/F/ || $flags =~ m/R/))
		{
		    if ($hash{$fid}->{etime} == 0)
		    {
			$hash{$fid}->{etime} = $time;
			my $dur = $hash{$fid}->{etime} - $hash{$fid}->{stime};
			if ($dur < 0.5)
			{
			    $blue ++;
			}
			else
			{
			    $red ++;
			}
			delete $hash{$fid};
		    }
		}
	    }
	}
    }
    for $c (keys %hash)
    {
	my $dur = $lasttime - $hash{$c}->{stime};
	my $s = $hash{$c}->{stime};
	if ($dur >= 0.5)
	{
	    $red++;
	}
	else
	{
	    $blue++;
	}
    }
}
# Read what's already in file
$host = hostname;
my @items = split /\./, $host;
my $exp = $items[1];
my $proj = $items[2];

# Fix capitalization issues
$plist = `ls /proj`;
my @items = split /\s+/, $plist;
for $i (@items)
{
    if (lc($i) eq $proj)
    {
	$proj = $i;
    }
}

# Fix more capitalization
$elist = `ls /proj/$proj/exp`;
my @items = split /\s+/, $elist;
for $i (@items)
{
    if (lc($i) eq $exp)
    {
	$exp = $i;
    }
}

my $filename = "/proj/" . $proj . "/exp/" . $exp . "/tbdata/score";
open(my $fh, "<", $filename);
my $line = <$fh>;
my @items = split / /,$line;
print "blue $blue red $red\n";
if ($#items == 3)
{
    $blue += $items[1];
    $red += $items[3];
} 
close($fh);
open(my $oh, ">", $filename);
print $oh "blue $blue red $red\n";
print "blue $blue red $red\n";
close($oh);
