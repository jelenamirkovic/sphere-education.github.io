#!/bin/bash

# copy and decompress a file from storage to /images directory
function usage
{
	echo "Try: sudo ./loadimage.sh imagename"
	echo "where imagename is one of:"
	echo " - act1.img (the startup company)"
	echo " - act2.img (the defense company)"
	echo " - act3.img (the millionaire)"
	echo
	echo "The image will be extracted into this directory"
	echo "along with the partition table information."
}

if [ ! -z $1 ]
then
	IMG=$1
else
	usage
	exit 1
fi

if [ ! -z $IMG ]
then
	echo "Loading image. Please wait..."
	if ! curl http://scratch/UCLAClass/forensics/$IMG.bz2 > /images/$IMG.bz2
	then
		echo "Couldn't download file!"
		exit 1
	fi
	
	# remove uncompressed image file if it exists
	if [ -e $IMG ];
	then
	  rm $IMG;
	fi

	echo "Decompressing image. Please wait..."
	bunzip2 $IMG.bz2
	echo "Done."
else
	echo "Image file '$IMG' does not exist."
	usage
	exit 1
fi
	
