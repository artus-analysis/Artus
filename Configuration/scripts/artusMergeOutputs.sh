#!/bin/bash

sed -i -e "s@srm://dcache-se-cms.desy.de:8443/srm/managerv2?SFN=@dcap://dcache-cms-dcap.desy.de/@g" ${FILE_NAMES}
sed -i -e "s@srm://grid-srm.physik.rwth-aachen.de:8443/srm/managerv2?SFN=@dcap://grid-dcap-extern.physik.rwth-aachen.de/@g" ${FILE_NAMES}
#sed -i -e "s@srm://dgridsrm-fzk.gridka.de:8443/srm/managerv2?SFN=@...@g" ${FILE_NAMES}

mkdir -p ${PROJECT_DIR}/merged/${DATASETNICK}
hadd.py -n 100 -a " -f" -t  ${PROJECT_DIR}/merged/${DATASETNICK}/${DATASETNICK}.root \"`cat ${FILE_NAMES}`\" --log-level debug && echo "success" > success

