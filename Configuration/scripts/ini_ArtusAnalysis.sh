#!/bin/bash


# get path of Artus installation
if [[ ${BASH_SOURCE[0]} == *"${CMSSW_BASE}/bin/"* ]]; then
	export ARTUSPATH=$CMSSW_BASE/src/Artus
else
	# get Artus dir relative to location of this script
	export ARTUSPATH=$(readlink -f $(dirname $(readlink -f ${BASH_SOURCE[0]}))/../..)
fi

export KAPPAPATH=$(readlink -f $ARTUSPATH/../Kappa)
export KAPPATOOLSPATH=$(readlink -f $ARTUSPATH/../KappaTools)

# configurations needed for compilation of C++ code
#export BOOSTPATH=$(ls /afs/cern.ch/cms/${SCRAM_ARCH}/external/boost/* -d | tail -n 1)/
export LD_LIBRARY_PATH="$KAPPATOOLSPATH/lib/:$KAPPAPATH/lib/:$LD_LIBRARY_PATH"

# useful to redirect messages
export USERPC=`who am i | sed 's/.*(\([^]]*\)).*/\1/g'`

if [[ `hostname` == *naf* ]]; then
	export ARTUS_WORK_BASE="/nfs/dust/cms/user/${USER}/artus/"
elif [[ `hostname` == *ekp* ]]; then
	export ARTUS_WORK_BASE="/storage/a/${USER}/artus/"
elif [[ `hostname` == *cern* ]]; then
	export ARTUS_WORK_BASE="/afs/cern.ch/work/${USER:0:1}/${USER}/artus/"
fi

