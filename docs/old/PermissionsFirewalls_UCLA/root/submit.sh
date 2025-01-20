#!/bin/bash

THING="$USER-permissions"
DATE=`date +%s`
TMP="/root/$THING"

echo "Creating submission file: '$TMP'"

sudo rm -rf $TMP
sudo mkdir $TMP
sudo cp -ar /admins $TMP
sudo cp -ar /home $TMP
sudo cp -ar /tpsreports $TMP
sudo cp -ar /ballots $TMP
sudo cp -a /etc/sudoers $TMP
sudo cp -a /etc/group $TMP
sudo cp -a /etc/passwd $TMP
sudo cp -a /etc/shadow $TMP
sudo cp -ar /root/firewall $TMP
sudo cp /root/permissions-answers.txt $TMP

sudo tar -cpzf ./$THING-$DATE.tar.gz $TMP

sudo rm -rf $TMP
echo "Done."
