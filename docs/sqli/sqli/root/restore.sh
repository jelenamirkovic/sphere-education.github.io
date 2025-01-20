#!/bin/bash

DATE=$(date -u +%s)
STO="/tmp/restore-$DATE"
INT="n" # default is not interactive
RES="y" # default is to restore
CHK="-avri"
OVR="-avr"
OPT=$OVR
#ARCHIVE="${1##/*/}"
ARCHIVE="$1"
TARBALL=${ARCHIVE##/*/} # just the tarball name
TGTDIR=${TARBALL%%.*}    # just the name of the tarball (w/o extension)

if [ ! -e $ARCHIVE ]; then
echo "DETERLab Restore v0.1"
echo 
echo "Usage: ./restore.sh <tarball.tar.gz>"
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
	if [[ $INT == "y" ]]; then
		echo "Restore $ARCHIVE? (y/n/q/a)"
		read RES
	fi
	if [[ $RES == "a" ]]; then
		INT="n"
		RES="y"
	fi

	if [[ $RES == "y" ]]; then
		if ! sudo $2; then
			echo "Error executing $2!"
		fi
	elif [[ $RES == "q" ]]; then
		echo "User requested abort!"
		Cleanup
		exit 0;
	else
		echo "Skipping $ARCHIVE..."
	fi
	
}

if [[ -d $STO ]]
then
	echo "Removing old directory $STO..."
	rm -rf $STO
fi

if ! sudo mkdir $STO; then
	echo "Couldn't create $STO!"
	exit 1;
fi

if ! sudo cp $ARCHIVE $STO; then
	echo "Couldn't copy $ARCHIVE to $STO!"
	exit 1;
fi

cd $STO
if ! sudo tar xzf $ARCHIVE; then
	echo "Couldn't extract $ARCHIVE!"
	exit 1;
fi

echo "This script will automatically overwrite all files."
echo "Press enter to continue or q to quit."
read FOO
if [[ $FOO == "q" ]]; then
	echo "Quitting..."
	Cleanup
	echo "Done."
	exit
fi

RES="y"
INT="n"

cd tmp/submit*

CheckDo "FCCU.php" "cp $OPT FCCU.php /usr/lib/cgi-bin/"
CheckDo "/root/submission" "cp $OPT submission /root/"

Cleanup
echo "Done."


