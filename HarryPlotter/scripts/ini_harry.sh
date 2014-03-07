#!/bin/bash
# This file might be merged with a general ini script

# This line fixes a bug in CMSSW6. The dateutils are not present there. This uses the dateutils from python 2.6
export PYTHONPATH=$PYTHONPATH:${CMSSW_RELEASE_BASE}/../../../../slc5_amd64_gcc462/external/py2-matplotlib/1.0.1/lib/python2.6/site-packages/

# Use this to open a root file directly in the TBrowser
rot()
{
    root -l $@ ${CMSSW_BASE}/src/Artus/Utility/scripts/tBrowser.C
}

# Use this to open the default data and mc files directly in a TBrowser
alias rooot='rot $DATA $MC'

