#!/bin/bash

if [ -n "$BASH_VERSION" ]; then

	# get path of Artus installation for bash
	if [[ ${BASH_SOURCE[0]} == *"${CMSSW_BASE}/bin/"* || ${BASH_SOURCE[0]} == *"${CMSSW_BASE}/src/Artus/"* ]]; then
		export ARTUSPATH=$CMSSW_BASE/src/Artus
	else
		# get Artus dir relative to location of this script for bash
		export ARTUSPATH=$(readlink -f $(dirname $(readlink -f ${BASH_SOURCE[0]}))/../..)
	fi

elif [ -n "$ZSH_VERSION" ]; then

	# get path of Artus installation for zsh
	if [[ ${0} == *"${CMSSW_BASE}/bin/"* || ${0} == *"${CMSSW_BASE}/src/Artus/"* ]]; then
		export ARTUSPATH=$CMSSW_BASE/src/Artus
	else
		# get Artus dir relative to location of this script for zsh
		export ARTUSPATH=$(readlink -f -- $(dirname -- "$(readlink -f -- "${0}")")/../..)
	fi

fi

# voms proxy path
export X509_USER_PROXY=$HOME/.globus/x509up

# useful to redirect messages
export USERPC=`who am i | sed 's/.*(\([^]]*\)).*/\1/g'`

if [[ `hostname` == *naf* ]]; then
	export ARTUS_WORK_BASE="/nfs/dust/cms/user/${USER}/artus/"
elif [[ `hostname` == *ekp* ]]; then
	export ARTUS_WORK_BASE="/storage/a/${USER}/artus/"
elif [[ `hostname` == *rwth* ]]; then
	export ARTUS_WORK_BASE="/net/scratch_cms3b/${USER}/artus/"
elif [[ `hostname` == *cern* ]]; then
	export ARTUS_WORK_BASE="/afs/cern.ch/work/${USER:0:1}/${USER}/artus/"
fi

# speed up compiling time # TODO: should only be a temporary fix
if [[ -e ${CMSSW_BASE}/config/toolbox/${SCRAM_ARCH}/tools/selected/gcc-cxxcompiler.xml ]]; then
	cp ${CMSSW_BASE}/config/toolbox/${SCRAM_ARCH}/tools/selected/gcc-cxxcompiler.xml ${CMSSW_BASE}/config/toolbox/${SCRAM_ARCH}/tools/selected/gcc-cxxcompiler.xml.backup
	sed -i -e "s@-fipa-pta@@g" ${CMSSW_BASE}/config/toolbox/${SCRAM_ARCH}/tools/selected/gcc-cxxcompiler.xml
fi

# grid-control import
export PYTHONPATH=${CMSSW_BASE}/src/grid-control/packages:${PYTHONPATH}
