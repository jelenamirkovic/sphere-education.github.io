#! /usr/bin/perl

$|=1;
use Class::Struct;
use Math::BigInt;
use Sys::Hostname;

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
	if ($items[0] =~ m/client/)
	{
	    next;
	}
	$access{$items[0]} = $items[1];
    } 
}
close($fh);
# Now randomly choose which clients red can access (2 out of 3)
# The remaining client will run logging
my %log;
for (my $i=1; $i<4; $i++)
{
    $log{"client" . $i} = 1;
}
# Get the time of experiment swap in and srand with that
my $file = "/proj/" . $proj . "/exp/" . $exp . "/tbdata/ltmap";
# Check if file exists
if (!(-e $file))
{
    print STDERR "Invalid Project and/or Experiment name";
    exit 0;
}
$mtime = (stat ($file))[9];    
srand($mtime);
for (my $i=0; $i<2; $i++)
{
    my $n;
    do{
	$n = "client" . (int(rand(3))+1);
    }while(exists($access{$n}));
    $access{$n} = "red";
    delete($log{$n});
}
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
    my $runcmd = "ssh -o StrictHostKeyChecking=no " . $node . "." . $exp . "." . $proj .  " \"sudo chmod a+w /usr/local/etc/emulab/rc/rc.testbed; sudo sed -i '/python/d' /usr/local/etc/emulab/rc/rc.testbed; sudo echo '" . $cmds{$node} . "' >> /usr/local/etc/emulab/rc/rc.testbed; sudo chmod 755 /usr/local/etc/emulab/rc/rc.testbed\"";
    system($runcmd);
}
# Copy all startup scripts to /etc/init on respective machines
for $node (keys %nodes)
{
    if ($node =~ m/client/)
    {
	if (exists($log{$node}))
	{
	    my $runcmd = "ssh -o StrictHostKeyChecking=no " . $node . "." . $exp . "." . $proj .  " \"sudo cp /share/education/ResilientServerGamified_USC/logger.conf /etc/init/\"";
	    print "$runcmd\n";
	    system($runcmd);
	}
	else
	{
	    my $runcmd = "ssh -o StrictHostKeyChecking=no " . $node . "." . $exp . "." . $proj .  " \"sudo cp /share/education/ResilientServerGamified_USC/client.conf /etc/init/\"";
	    print "$runcmd\n";
	    system($runcmd);
	}
    }
    if ($node =~ m/server/)
    {
	my $runcmd = "ssh -o StrictHostKeyChecking=no " . $node . "." . $exp . "." . $proj .  " \"sudo cp /share/education/ResilientServerGamified_USC/server.conf /etc/init/\"";
	print "$runcmd\n";
	system($runcmd);
    }
}
# Now reboot all machines
my $runcmd = "/usr/testbed/bin/node_reboot -e " . $proj . "," . $exp;
print "$runcmd\n";
system($runcmd);

