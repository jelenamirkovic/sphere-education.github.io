#! /usr/bin/perl

$|=1;
use Class::Struct;
use Math::BigInt;
use Sys::Hostname;

# Arguments are experiment and project names
my $exp = $ARGV[0];
my $proj = $ARGV[1];

print "Setting up the competition";

# Extract names of all nodes in the experiment                                                                                  
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

# Detect all team UNIX group names. All teams will intially                                                                   
# have access to all nodes                                                                                                    
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

# Find out which group names belong to blue and red team for THIS experiment                                                  
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

# Find out which teams can access which machines and remember                                                                 
# this in access table                                                                                                        
my %access;
my $filename = "/proj/" . $proj . "/exp/" . $exp . "/tbdata/bluered";
open(my $fh, "<", $filename);
while (my $line = <$fh>)
{
    $line =~ s/\n//;
    my @items = split / /,$line;
    $access{$items[0]} = $items[1];
}
close($fh);

# If any logging is needed remember the log machine's name here                                                               
# my %log;                                                                                                                    
# $log{$logmachinename} = 1;                                                                                                  

# Now ensure that access limitations will persist through reboots                                                             
# First build the command to restrict access to nodes. This command                                                           
# is node-specific                                                                                                            
print "Limiting access to nodes";

my %cmds;
for $node (keys %nodes)
{
    my $cmd=$proj;
    for $g (keys %groups)
    {
        if (!exists($access{$node}) || $g ne $teams{$access{$node}})
        {
            $cmd = $cmd . " " . $g;
        }
    }
    $cmds{$node} = $cmd;
}

# Now store this command in a file that will be executed on reboots                                                           
for $node (keys %nodes)
{
    my $runcmd1 = "ssh -o StrictHostKeyChecking=no " . $node . "." . $exp . "." . $proj .  " \"sudo chmod a+w /usr/local/etc/emulab/rc/rc.testbed\"";
    system($runcmd1);
    my $runcmd2 = "ssh -o StrictHostKeyChecking=no " . $node . "." . $exp . "." . $proj .  " \"sudo /share/education/CTF/restrict.pl " . $cmds{$node} . "\"";
    system($runcmd2);

}


