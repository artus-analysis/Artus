#!/bin/sh
set -e # exit on errors

export CWD="."
while getopts :c:j:d: OPTION
do
	case "${OPTION}"
	in
		c) export CMSSW_BASE=${OPTARG};;
		d) export CWD=${OPTARG};;
	esac
done
export ARGS=${@:$OPTIND}

echo "setup..."
export SCRAM_ARCH=slc6_amd64_gcc530
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh

cd ${CMSSW_BASE}/src
eval `scramv1 runtime -sh`
source ${CMSSW_BASE}/src/Artus/HarryPlotter/scripts/ini_harry_cmssw.sh
echo "setup done"

echo "plotting..."
cd $CWD
pwd
echo "harry.py $ARGS"
harry.py $ARGS
echo "plotting done"

exit $?
