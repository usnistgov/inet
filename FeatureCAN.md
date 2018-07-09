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
	
	• We have created the src/base/Inet_DEFS.h to replace the fico4omnet_Defs.h library. 
	• We have replaced every fico4omnet_Defs.h library import with the the src/base/Inet_DEFS.h library import. 
	
	• We have mnaually added the CAN features to the (.oppfeatures) file. 
	
	• In inet/examples : CAN examples use the notation src/nodes when calling files under this directory, must be chanegd to src/node to adapt to the destination package.

        • Setting the environement variables : 
	
	- OMNETPP_CONFIGFILE=/opt/omnetpp/omnetpp-5.2.1/Makefile.inc
	- PATH=$PATH:/opt/omnetpp/omnetpp-5.2.1/bin/
 
        • Configuring the features 
	
	- ./inet_featuretool disable wirelesstutorial
	- ./inet_featuretool disable wirelessshowcases
	- ./inet_featuretool disable visualizationtutorial
	- ./inet_featuretool disable configuratortutorial
	- ./inet_featuretool disable visualizershowcases
	- ./inet_featuretool reset

        • Pushing all the changes to the Git Repository. 

	- git add*
	- git commit
	- git push
	- All the changes are committed as can_commit, can_commit2, and can_commit3 in usnistgov/inet under the branch feature/can.

# Clone and Build steps : 

	- git clone -b feature/can https://github.com/usnistgov/inet.git
	- make makefiles
	- make

