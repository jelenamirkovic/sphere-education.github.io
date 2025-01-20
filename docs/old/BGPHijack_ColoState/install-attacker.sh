#!/bin/bash

#####
#  BGP Prefix Hijack Attack
#####
##
# Authors: Vamsi Kambhampati & Dr. Daniel Massey
# Author Org: Colorado State University
# Created On: 11/4/2011
##

# Customization script for the attacker

# installation needs superuser privilege
if [ "$(id -u)" != "0" ]; then
  echo "This script must be run as root"
  exit 1
fi

export LAB="BGPHijack_ColoState"
export BASE="/share/education/$LAB"
export CONFS="$BASE/confs"

MYHOST=${HOSTNAME%%.*}

pushd $BASE

#####
echo "Installing FTP Server (vsftpd)"
apt-get -y install vsftpd

echo "Configuring FTP Server"
if [ -e $BASE/confs/$MYHOST/vsftpd ]; then
  cp -f $BASE/confs/$MYHOST/vsftpd/vsftpd.conf /etc/
  cp -f $BASE/confs/$MYHOST/vsftpd/README /srv/ftp/
  service vsftpd restart
else
  echo "Missing FTP server configuration file for $MYHOST"
  echo "  -> Not configuring FTP server on $MYHOST"
fi

