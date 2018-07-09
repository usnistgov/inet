# Context

## NIST/EL : Autonomous Vehicule Project 

### UCEF Environement

# CAN Technology

## Technology 

## How it works

## Usecases

# FeatureCAN branch : Modifications made to the original inet-framework/inet project. 

We have adapted the FICO4OMNET-CAN package (http://git.inet.haw-hamburg.de/core/public/FiCo4OMNeT) to OMNET ++ 5.2.1 & the latest INET Framework version 3.6.4 so that we can use these experiments within the UCEF Environement VM. 

Changes we made to the FICO4OMNET-CAN package : 

	• We have changed the namespace from fico4omnet to inet in every source code file in the following folders and files 
	
	-  inet/src/application/* 
	-  inet/src/node/can/* 
	-  inet/src/linklayer/can/*
	-  inet/src/buffer/can/*
	-  inet/examples/can/*
	-  inet/examples/package.ned
	
	• We have created the src/base/Inet_DEFS.h file which replaces every call of the old fico4omnet_Defs.h in every file that contains the latter.
	
	• We have added the CAN features to the (.oppfeatures)
	
	• In inet/examples : CAN examples use the notation src/nodes when calling files under this directory, must be chanegd to src/node to adapt to the destination package.

 
All the changes are committed as can_commit, can_commit2, and can_commit3 in usnistgov/inet under the branch feature/can. 


# Step by step to run the CAN examples withing INET Vx and OMNET Vy 


git add*
git commit
git push


OMNETPP_CONFIGFILE=/opt/omnetpp/omnetpp-5.2.1/Makefile.inc
PATH=$PATH:/opt/omnetpp/omnetpp-5.2.1/bin/


./inet_featuretool disable wirelesstutorial
./inet_featuretool disable wirelessshowcases
./inet_featuretool disable visualizationtutorial
./inet_featuretool disable configuratortutorial
./inet_featuretool disable visualizershowcases

./inet_featuretool reset


make makefiles
make
