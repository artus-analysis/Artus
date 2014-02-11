#!/bin/bash
# This file might be merged with a general ini script

# This line fixes a bug in CMSSW6. The dateutils are not present there. This uses the dateutils from python 2.6
export PYTHONPATH=$PYTHONPATH:${CMSSW_RELEASE_BASE}/../../../../slc5_amd64_gcc462/external/py2-matplotlib/1.0.1/lib/python2.6/site-packages/

# every user must set his default ntuple location, this is a fallback for testing
# this helps to keep paths out of the code
export MC=/home/berger/excalibur/work/mc.root
export DATA=/home/berger/excalibur/work/data.root

export USERPC=`who am i | sed 's/.*(\([^]]*\)).*/\1/g'`
export SYNCDIR=${CMSSW_BASE}/src/Artus/Plotting/out/websync

# Set some user specific variables
if [ $USER = "bergerxy" ]; then
    export PATH=/home/berger/sw/bin:/home/berger/sw/grid-control:$PATH
    export DATA=`readlink $CMSSW_BASE/src/Artus/Plotting/data/data.root`
    export MC=`readlink $CMSSW_BASE/src/Artus/Plotting/data/mc.root`
fi

# Use this to open a root file directly in the TBrowser
rot()
{
    root -l $@ ${CMSSW_BASE}/Plotting/scripts/tbrowser.cxx
}

# Use this to open the default data and mc files directly in a TBrowser
alias rooot='rot $DATA $MC'

