#! /usr/bin/perl

$|=1;
use Class::Struct;
use Math::BigInt;
use Sys::Hostname;

# Arguments are experiment and project names
my $exp = $ARGV[0];
my $proj = $ARGV[1];

$cmd = "sudo bash /share/education/ResilientServerGamified_USC/gameon/flow_stats/install";
system("ssh -o StrictHostKeyChecking=no gateway.$exp.$proj '$cmd'");
system("ssh -o StrictHostKeyChecking=no client1.$exp.$proj '$cmd'");
$cmd = "bash /share/education/ResilientServerGamified_USC/gameon/logging.sh";
system("ssh -o StrictHostKeyChecking=no gateway.$exp.$proj '$cmd' &");
system("ssh -o StrictHostKeyChecking=no client1.$exp.$proj '$cmd' &");
