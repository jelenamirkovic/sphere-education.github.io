#!/bin/bash
apt-get install bind9 -y
cp /proj/USC430/named.conf.local /etc/bind/named.conf.local
mkdir /etc/bind/zones
cp /proj/USC430/blue.net.db /etc/bind/zones
cp /proj/USC430/resolv.conf /etc
/etc/init.d/bind9 restart
