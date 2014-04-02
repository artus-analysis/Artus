#!/bin/bash

# configurations needed for compilation of C++ code
#export BOOSTPATH=$(ls /afs/cern.ch/cms/${SCRAM_ARCH}/external/boost/* -d | tail -n 1)/
export LD_LIBRARY_PATH="$CMSSW_BASE/src/KappaTools/lib/:$CMSSW_BASE/src/Kappa/lib/:$LD_LIBRARY_PATH"

# useful to rediect messages
export USERPC=`who am i | sed 's/.*(\([^]]*\)).*/\1/g'`

# set the environment
if [[ `hostname` == *naf* ]]; then
	export ARTUS_WORK_BASE="/nfs/dust/cms/user/${USER}/artus/"
elif [[ `hostname` == *ekp* ]]; then
	export ARTUS_WORK_BASE="/storage/a/${USER}/artus/"
elif [[ `hostname` == *cern* ]]; then
	export ARTUS_WORK_BASE="/afs/cern.ch/work/${USER:0:1}/${USER}/artus/"
fi

# init HarryPlotter
source $CMSSW_BASE/src/Artus/HarryPlotter/scripts/ini_harry.sh

