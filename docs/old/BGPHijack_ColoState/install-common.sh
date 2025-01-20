#!/bin/bash

#####
#  BGP Prefix Hijack Attack
#####
##
# Authors: Vamsi Kambhampati & Dr. Daniel Massey
# Author Org: Colorado State University
# Created On: 10/25/2011
##

# Common installation script for all nodes

# installation needs superuser privilege
if [ "$(id -u)" != "0" ]; then
  echo "This script must be run as root"
  exit 1
fi

export LAB="BGPHijack_ColoState"
export BASE="/share/education/$LAB"
export CONFS="$BASE/confs"

MYHOST=${HOSTNAME%%.*}

#####

# lockout users from logging in when installation is in progress
#  set to 1 to enable (0 to disable)
LOCKOUT=0

if [ -e $BASE/helper-functions ]; then
  source $BASE/helper-functions
else
  # cannot execute lockout functions if helper routines are not available
  LOCKOUT=0
fi

pushd $BASE

#####

echo
echo ">>> Beginning additional configuration of nodes for $LAB"
echo

# Ubuntu1004-TLS image comes with broken quagga installation. Force re-install
echo "Installing Quagga"
apt-get update
DEBIAN_FRONTEND=noninteractive apt-get -y install --reinstall quagga
cp $BASE/confs/quagga /etc/init.d/quagga
cp /etc/pam.d/su /etc/pam.d/quagga
echo "Installing Traceroute"
DEBIAN_FRONTEND=noninteractive apt-get -y install traceroute

echo "Configuring Quagga"
if [ -e $BASE/confs/$MYHOST/quagga ]; then
  cp -f $BASE/confs/$MYHOST/quagga/* /etc/quagga/
  echo "Starting Quagga..."
  addgroup quagga
  addgroup quaggavty
  useradd -g quagga quagga
  usermod -a -G quaggavty quagga
  chown quagga.quagga /etc/quagga/zebra.conf
  chown quagga.quagga /etc/quagga/bgpd.conf
  chown quagga.quaggavty /etc/quagga/vtysh.conf
  service zebra restart
  service bgpd restart
else
  echo "Missing Quagga configuration files for $MYHOST"
  echo "  -> Not configuring Quagga on $MYHOST"
fi

# machine specific installation steps
#  only executed if a script named "install-$hostname" exists
CUSTOMSCRIPT="$BASE/install-${MYHOST}.sh"
if [ -e $CUSTOMSCRIPT ]; then
    echo ">>> Performing host specific installation tasks for $MYHOST..."
    $CUSTOMSCRIPT >> /tmp/install.log
    echo ">>> Done host specific installation tasks for $MYHOST..."
fi

echo ">>> Done additional configuration of nodes for $LAB"

