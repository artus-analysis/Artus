#!/bin/bash
echo "userjob. Setting up CMSSW"
pwd
pushd .
cd $1 
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
eval `scramv1 runtime -sh`
source $CMSSW_BASE/src/Artus/Configuration/scripts/ini_ArtusAnalysis.sh
popd
pwd
arguments="${@:2}"
echo $arguments
python $arguments
