#!/bin/bash

DATE=$( date -u +%s )
STO="/tmp/restore-$DATE"
INT="n" # default is not interactive
RES="y" # default is to restore
CHK="-avri"
OVR="-avr"
OPT="-avri"
ARCHIVE="${1##/*/}"

if [ ! -e $ARCHIVE ]; then
echo "DETERLab Restore v0.1"
echo 
echo "Usage: sudo ./restore.sh <tarball.tar.gz>"
echo
echo "Copy the tarball into the /root directory first, e.g.:"
echo "cp /users/USERNAME/root-permissions-XXXXXXX.tar.gz /root"
echo
echo "This script will restore files in a tarball created by the submit.sh"
echo "submission script."
exit 0;
fi

function Cleanup
{
	sudo rm -rf $STO;
}

function CheckDo
# check to see if user is in interactive mode and do action
# $1 -- the name of the part being restored
# $2 -- the commands used to restore resource
{
	if [ $INT = "y" ]; then
		echo "Restore $1? (y/n/q/a)"
		read RES
	fi
	if [ $RES = "a" ]; then
		INT="n"
		RES="y"
	fi

	if [ $RES = "y" ]; then
		if ! sudo $2; then
			echo "Error executing $2!"
		fi
	elif [ $RES = "q" ]; then
		echo "User requested abort!"
		Cleanup
		exit 0;
	else
		echo "Skipping $1..."
	fi
	
}

if ! sudo mkdir $STO; then
	echo "Couldn't create $STO!"
	exit 1;
fi

if ! sudo cp $1 $STO; then
	echo "Couldn't copy $ARCHIVE to $STO!"
	exit 1;
fi

cd $STO
if ! sudo tar xvzpf $ARCHIVE; then
	echo "Couldn't extract $ARCHIVE!"
	exit 1;
fi

echo "Do you want to automatically overwrite files? (y/N)"
read OPT
OPT=${OPT:-"n"} # default
if [ $OPT = "y" ]; then
	OPT=$OVR
else
	OPT=$CHK
fi


echo "Do you want to extract the files interactively? (y/N)"
read INT
INT=${INT:-"n"} # return "n" if INT is null

cd root/$USER-permissions

CheckDo "/admins" "cp $OPT admins /"
sleep 1
CheckDo "/home" "cp $OPT home /"
sleep 1
CheckDo "/tpsreports" "cp $OPT tpsreports /"
sleep 1
CheckDo "/ballots" "cp $OPT ballots /"
sleep 1
CheckDo "sudoers" "cp $OPT sudoers /etc/"
sleep 1
CheckDo "group" "cp $OPT group /etc/"
sleep 1
CheckDo "passwd" "cp $OPT passwd /etc/"
sleep 1
CheckDo "shadow" "cp $OPT shadow /etc/"
sleep 1
CheckDo "firewall" "cp $OPT firewall /root/"
sleep 1
CheckDo "permissions-answers.txt" "cp $OPT permissions-answers.txt /root/"
sleep 1

Cleanup
echo "Done."


