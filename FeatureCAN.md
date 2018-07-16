
# Modifications 

* fork from inet-framework/inet in usnistgov/inet
* In the UCEF VM , clone the repos below in the following folder :  /home/vagrant 
```
	- (a) ( git clone http://git.inet.haw-hamburg.de/core/public/FiCo4OMNeT ) (/home/vagrant/﻿FiCo4OMNeT)
	- (b) ( git clone https://github.com/inet-framework/inet.git           ) (/home/vagrant/inet)
```
* Copy/Merge the following folders and files from (a) to (b) 
```
	- /home/vagrant/﻿FiCo4OMNeT/src/﻿fico4omnet/application/can/*     ==>    /home/vagrant/inet/src/inet/application/
	- /home/vagrant/﻿FiCo4OMNeT/src/﻿fico4omnet/nodes/can/*              ==>   /home/vagrant/inet/src/inet/node/
	- /home/vagrant/﻿FiCo4OMNeT/src/﻿fico4omnet/linklayer/can/*          ==>   /home/vagrant/inet/src/inet/linklayer/
	- /home/vagrant/﻿FiCo4OMNeT/src/﻿fico4omnet/buffer/can/*              ==>   /home/vagrant/inet/src/inet/buffer/
	- /home/vagrant/﻿FiCo4OMNeT/examples/can/*                                      ==>   /home/vagrant/inet/examples/
	- /home/vagrant/﻿FiCo4OMNeT/examples/package.ned                          ==>   /home/vagrant/inet/examples/
```
* Don’t copy FlexRay files from (a) as we are interested in CAN files only. 
* Create / Modify the following files and folders
	
	- In (b) Create /home/vagrant/inet/src/base/Inet_DEFS.h to replace the /home/vagrant/inet/src/base/fico4omnet_Defs.h library file. 
	- Use the following command to find all occurrences of fico4omnet namespace and fico4omnet_Defs.h library import : 
	      grep -rnw '/home/vagrant/inet' -e 'fico4omnet'
	- Replace fico4omnet_Defs.h library import with the the src/base/Inet_DEFS.h library import. 
	-  Add the CAN features to the /home/vagrant/inet/.oppfeatures file. 
```
	<feature
        id="CAN_common"
        name="CAN Common"
        description = "The common part of CAN implementation"
        initiallyEnabled = "true"
        requires = ""
        labels = ""
        nedPackages = "
                       inet.applications.can
                       inet.buffer.can
                       inet.bus.can
                       inet.linklayer.can
                       inet.node.can
                       inet.scheduler.can
                      "
        extraSourceFolders = ""
        compileFlags = "-DWITH_CAN_COMMON"
        linkerFlags = ""
        />
    <feature
        id="CAN_examples"
        name="CAN examples"
        description = "Examples for the CAN implementation"
        initiallyEnabled = "true"
        requires = "CAN_common"
        labels = ""
        nedPackages = "
                       inet.examples.can
                      "
        extraSourceFolders = ""
        linkerFlags = ""
        />
 ```   

* Configuring the features 
```
	./inet_featuretool reset
	./inet_featuretool disable wirelesstutorial
	./inet_featuretool disable wirelessshowcases
	./inet_featuretool disable visualizationtutorial
	./inet_featuretool disable configuratortutorial
	./inet_featuretool disable visualizershowcases
```   

* Push changes to the git repo : 
```
	- git add *
	- git commit
	- git push
```

# Run CAN examples (ex : arbitration)

* Clone feature/can to /home/vagrant

```
git clone -b feature/can https://github.com/usnistgov/inet.git

```

* Set the environment variables
```
	-  OMNETPP_CONFIGFILE=/opt/omnetpp/omnetpp-5.2.1/Makefile.inc
	-  PATH=$PATH:/opt/omnetpp/omnetpp-5.2.1/bin/
```	

* build project 

```
	- cd /home/vagrant/inet/
	- make makefiles 
	- make
```

* In eclipse : 
```
	- File => import => Existing project into workspace => Select root directory => Browse => /home/vagrant/inet/
	- When the project is imported :  Inet -> Examples -> can -> arbitration
	- right click on omnetpp.ini -> run as omnetpp++ Simulation. 
```
