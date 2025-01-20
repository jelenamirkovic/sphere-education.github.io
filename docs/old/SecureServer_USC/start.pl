#! /usr/bin/perl

$|=1;
use Class::Struct;
use Math::BigInt;
use Sys::Hostname;

$usage = "$0 exp proj\n";

if ($#ARGV < 1)
{
    print $usage;
    exit 0;
}
# Arguments are experiment and project names
my $exp = $ARGV[0];
my $proj = $ARGV[1];

# Remember nodes
my %nodes;
my $filename = "/proj/" . $proj . "/exp/" . $exp . "/tbdata/ltmap";
open(my $fh, "<", $filename);
while (my $line = <$fh>)
{
    $line =~ s/\n//;
    if ($line =~ m/^h /)
    {
	my @items = split /\s/, $line;
	$nodes{$items[1]} = 1;
    }
}

# Remember groups
my %groups;
open(my $fh, "<", "/etc/group");
while (my $line = <$fh>)
{
    $line =~ s/\n//;
    my @items = split /\:/,$line;
    my $pref = substr($proj, 0, 3);
    if ($items[0] !~ m/$pref\-.*team\_\d+\_\d+$/)
    {
	next;
    }
    if ($#items == 3)
    {
	# Get group name from UNIX group name
	my $g = substr($items[0], 4);
	$groups{$g} = 1;
    }
}

# Remember teams
my %teams;
my $filename = "/proj/" . $proj . "/exp/" . $exp . "/tbdata/teams";
open(my $fh, "<", $filename);
while (my $line = <$fh>)
{
    $line =~ s/\n//;
    my @items = split / /,$line;
    if ($#items == 1)
    {
	$teams{$items[1]} = $items[0];
    }
}


# Remember which teams can access which machines
my %access;
my $filename = "/proj/" . $proj . "/exp/" . $exp . "/tbdata/bluered";
open(my $fh, "<", $filename);
while (my $line = <$fh>)
{
    $line =~ s/\n//;
    my @items = split / /,$line;
    if ($#items == 1)
    {
	# In this exercise, because we want to randomize access to clients
	# ignore client info if any
	if ($items[0] =~ m/red[1-3]/)
	{
	    next;
	}
	$access{$items[0]} = $items[1];
    } 
}
close($fh);
# Get the time of experiment swap in and srand with that
my $file = "/proj/" . $proj . "/exp/" . $exp . "/tbdata/ltmap";
$mtime = (stat ($file))[9];    
srand($mtime);
$access{'redserver'} = "red";
$access{'blueserver'} = "blue";
$access{'bluegateway'} = "blue";
$log{'redgateway'} = 1;
# Now ensure that remove_access is called as needed, on reboot
my %cmds;
for $node (keys %nodes)
{
    my $cmd="/usr/bin/python /share/education/CTF/remove_group.py " . $proj;
    for $g (keys %groups)
    {
	if (!exists($access{$node}) || $g ne $teams{$access{$node}})
	{
	    $cmd = $cmd . " " . $g;
	}
    }
    $cmds{$node} = $cmd;
}
for $node (keys %nodes)
{
    my $runcmd = "ssh -o StrictHostKeyChecking=no " . $node . "." . $exp . "." . $proj .  " \"sudo chmod a+w /usr/local/etc/emulab/rc/rc.testbed; sudo echo '" . $cmds{$node} . "' >> /usr/local/etc/emulab/rc/rc.testbed; sudo chmod 755 /usr/local/etc/emulab/rc/rc.testbed\"";
    system($runcmd);
}
# Copy all startup scripts to /etc/init on respective machines
for $node (keys %nodes)
{
    if (exists($log{$node}))
    {
	my $runcmd = "ssh -o StrictHostKeyChecking=no " . $node . "." . $exp . "." . $proj .  " \"sudo cp /share/education/SecureServer_USC/scorer.conf /etc/init/\"";
	print "$runcmd\n";
	system($runcmd);
    }
    my $runcmd = "ssh -o StrictHostKeyChecking=no " . $node . "." . $exp . "." . $proj .  " \"sudo cp /share/education/SecureServer_USC/host.conf /etc/init/\"";
    print "$runcmd\n";
    system($runcmd);
}
# Now reboot all machines
my $runcmd = "/usr/testbed/bin/node_reboot -e " . $proj . "," . $exp;
print "$runcmd\n";
system($runcmd);

