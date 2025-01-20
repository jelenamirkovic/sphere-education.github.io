************ List of files ******************************
paws.h		header file

server.c	paws_server source code

client.c	paws_client source code

makefile	makefile

ASLinks.dat	list of inter-as links

BGPAtom2AS.dat	list of BGP atoms

IPRangeTable.dat	list of IP ranges

sortedASbySize.dat	list of ASes

paws_RT.dat	inter-AS routing tables

backgroundTraffic.dat	background traffic definition

as_slammer_vulnerable_ratio.txt	distribution of slammer-worm vulnerable hosts

nsFile.tcl	sample ns file for experiment on Emulab/Deter testbed with 5 nodes

slammer.sh	shell script to run slammer worm simulation


All the data files are created based on RouteViews data of Slammer outbreak day (Jan 25, 2003)

*********** Steps to run a simulation *************************
Step 1:
	node0#> make
	to bulid paws_cleint and paws_server programs
Step 2:
	node0#> ./paws_server
	to run the server program on node0
Step 3:
	node0#> ./slammer.sh
	to run the client programs on other nodes


