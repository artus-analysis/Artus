#!/bin/sh

/etc/cvmfs/run-cvmfs.sh

# export SCRAM_ARCH=slc6_amd64_gcc481
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch

mkdir -p /home/build && cd /home/build


. $VO_CMS_SW_DIR/cmsset_default.sh
scram project ${TEST_CMSSW_VERSION}

cd ${TEST_CMSSW_VERSION}

cmsenv

cd src/

git clone https://github.com/KappaAnalysis/Kappa.git

mkdir Artus
cp -r /home/travis/* Artus/

echo "# ================= #"
echo "# Building in CMSSW #"
echo "# ================= #"

scram b -v -j 2 || exit 1
