#!/bin/sh
set -e # exit on errors

export SCRAM_ARCH=slc6_amd64_gcc491
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh

# set up CMSSW release area
if [ ! -e CMSSW_7_4_7/src ]
then
	scram project CMSSW CMSSW_7_4_7
fi
cd CMSSW_7_4_7/src
eval `scram runtime -sh`

# check out HarryPlotter
if [ ! -e Artus/HarryPlotter ]
then
	git clone https://github.com/artus-analysis/Artus.git -n
	cd Artus
	git remote add -f Artus https://github.com/artus-analysis/Artus.git
	git config core.sparsecheckout true
	echo "/HarryPlotter/" >> .git/info/sparse-checkout
	echo "/Utility/python/" >> .git/info/sparse-checkout
	echo "/Utility/scripts/" >> .git/info/sparse-checkout
	echo "/Configuration/scripts/ini_artus_python_standalone.sh" >> .git/info/sparse-checkout
	echo "Example/scripts/generateCorrelatedGaussians.py" >> .git/info/sparse-checkout
	git fetch origin
	git merge origin/master
	git read-tree --empty
	git read-tree -mu HEAD

	# compile everything
	cd ${CMSSW_BASE}/src
	scram b -j `grep -c ^processor /proc/cpuinfo`
fi

export ARTUS_PATH="${CMSSW_BASE}/src/Artus"
source ${CMSSW_BASE}/src/Artus/HarryPlotter/scripts/ini_harry_cmssw.sh

set +e
