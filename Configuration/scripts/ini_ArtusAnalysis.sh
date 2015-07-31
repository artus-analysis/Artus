#!/bin/bash

# get path of Artus installation
if [[ ${BASH_SOURCE[0]} == *"${CMSSW_BASE}/bin/"* || ${BASH_SOURCE[0]} == *"${CMSSW_BASE}/src/Artus/"* ]]; then
	export ARTUSPATH=$CMSSW_BASE/src/Artus
else
	# get Artus dir relative to location of this script
	export ARTUSPATH=$(readlink -f $(dirname $(readlink -f ${BASH_SOURCE[0]}))/../..)
fi

export KAPPAPATH=$(readlink -f $ARTUSPATH/../Kappa)
export KAPPATOOLSPATH=$(readlink -f $ARTUSPATH/../KappaTools)

# configurations needed for compilation of C++ code
#export BOOSTPATH=$(ls /afs/cern.ch/cms/${SCRAM_ARCH}/external/boost/* -d | tail -n 1)/
export LD_LIBRARY_PATH="$KAPPATOOLSPATH/lib:$KAPPAPATH/lib:$LD_LIBRARY_PATH"

# voms proxy path
export X509_USER_PROXY=$HOME/.globus/x509up

# useful to redirect messages
export USERPC=`who am i | sed 's/.*(\([^]]*\)).*/\1/g'`

if [[ `hostname` == *naf* ]]; then
	export ARTUS_WORK_BASE="/nfs/dust/cms/user/${USER}/artus/"
elif [[ `hostname` == *ekp* ]]; then
	export ARTUS_WORK_BASE="/storage/a/${USER}/artus/"
elif [[ `hostname` == *cern* ]]; then
	export ARTUS_WORK_BASE="/afs/cern.ch/work/${USER:0:1}/${USER}/artus/"
fi

# speed up compiling time # TODO: should only be a temporary fix
if [[ -e ${CMSSW_BASE}/config/toolbox/${SCRAM_ARCH}/tools/selected/gcc-cxxcompiler.xml ]]; then
	cp ${CMSSW_BASE}/config/toolbox/${SCRAM_ARCH}/tools/selected/gcc-cxxcompiler.xml ${CMSSW_BASE}/config/toolbox/${SCRAM_ARCH}/tools/selected/gcc-cxxcompiler.xml.backup
	sed -i -e "s@-fipa-pta@@g" ${CMSSW_BASE}/config/toolbox/${SCRAM_ARCH}/tools/selected/gcc-cxxcompiler.xml
fi

