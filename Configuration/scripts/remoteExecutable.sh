#!/bin/bash
##################################################
# remote executable that is called by grid-control
# functionality is kept to a minimum. Most of it is
# done inside the wrapper script.
##################################################
#DATASETNICK="DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV"
#FILE_NAMES="/storage/a/friese/skimming/2014-03-05-sync-exercise/DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV/kappa_DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV_100.root /storage/a/friese/skimming/2014-03-05-sync-exercise/DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV/kappa_DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV_101.root /storage/a/friese/skimming/2014-03-05-sync-exercise/DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV/kappa_DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV_52.root /storage/a/friese/skimming/2014-03-05-sync-exercise/DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV/kappa_DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV_53.root /storage/a/friese/skimming/2014-03-05-sync-exercise/DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV/kappa_DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV_54.root /storage/a/friese/skimming/2014-03-05-sync-exercise/DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV/kappa_DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV_55.root /storage/a/friese/skimming/2014-03-05-sync-exercise/DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV/kappa_DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV_56.root /storage/a/friese/skimming/2014-03-05-sync-exercise/DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV/kappa_DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV_57.root /storage/a/friese/skimming/2014-03-05-sync-exercise/DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV/kappa_DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV_58.root /storage/a/friese/skimming/2014-03-05-sync-exercise/DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV/kappa_DoubleMu_PFembedded_Run2012A_22Jan2013_muelec_8TeV_59.root"
#CONFIG_FILE="../data/ArtusWrapperConfigs/smHtautau.cfg"

# construct command
COMMAND=$EXECUTABLE".py" #Auslesen
COMMAND=$COMMAND" "
COMMAND=$COMMAND"-c "$JSON_CONFIG
COMMAND=$COMMAND" "
COMMAND=$COMMAND"--nick "$DATASETNICK
COMMAND=$COMMAND" "
COMMAND=$COMMAND"-i "${FILE_NAMES//[,\"]/}

# debug output that is save in gc.stdout
echo "-------------------------------------------------"
echo "Starting main executable with the following command:"
echo $COMMAND
echo "The logfile of this job is called "$DATASETNICK"_job_"$MY_JOBID"_log.txt"
echo "-------------------------------------------------"

# create logfile that will be copied together with the output and pipe everythin into it
LOGFILE=log.txt
touch $LOGFILE

#Set searchpath for external libraries # might be replaced by executing a part of the ini scripts
LD_LIBRARY_PATH="$CMSSW_BASE/src/KappaTools/lib/:$CMSSW_BASE/src/Kappa/lib/:$LD_LIBRARY_PATH"

echo "start wrapper with the command:">>$LOGFILE
eval $COMMAND>>$LOGFILE
echo "wrapper ended with status:"$?>>$LOGFILE

exit 0


