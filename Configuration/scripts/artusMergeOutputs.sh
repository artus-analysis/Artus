#!/bin/bash

export FILE_NAMES=`echo ${FILE_NAMES} | sed -e "s@srm://dcache-se-cms.desy.de:8443/srm/managerv2?SFN=@dcap://dcache-cms-dcap.desy.de/@g"`
export FILE_NAMES=`echo ${FILE_NAMES} | sed -e "s@srm://grid-srm.physik.rwth-aachen.de:8443/srm/managerv2?SFN=@dcap://grid-dcap-extern.physik.rwth-aachen.de/@g"`
#export FILE_NAMES=`echo ${FILE_NAMES} | sed -e "s@srm://dgridsrm-fzk.gridka.de:8443/srm/managerv2?SFN=@...@g"`

hadd -f ${DATASETNICK}.root ${FILE_NAMES}

