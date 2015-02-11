#!/bin/bash
# This file might be merged with a general ini script

source $CMSSW_BASE/src/Artus/HarryPlotter/scripts/ini_harry_cmssw.sh

#add harry to PATH
export PATH=$CMSSW_BASE/src/Artus/HarryPlotter/scripts:$PATH

# Use this to open a root file directly in the TBrowser
rot()
{
    root -l $@ ${CMSSW_BASE}/src/HarryPlotter/Utility/scripts/tBrowser.C
}
