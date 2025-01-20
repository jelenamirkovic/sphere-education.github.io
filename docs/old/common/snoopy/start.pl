#! /usr/bin/perl

$|=1;
use Class::Struct;
use Math::BigInt;
use Sys::Hostname;

# No arguments needed
my $cmd="/usr/bin/perl /usr/local/src/snoopy/logsnoopy.pl &";
my $runcmd = "sudo chmod a+w /usr/local/etc/emulab/rc/rc.testbed; sudo sed -i '/python/d' /usr/local/etc/emulab/rc/rc.testbed; sudo echo '" . $cmd . "' >> /usr/local/etc/emulab/rc/rc.testbed; sudo chmod 755 /usr/local/etc/emulab/rc/rc.testbed";
print $runcmd;
system($runcmd);
