#!/bin/bash

if [[ -z ${HARRY_USERPC} ]]; then
	export HARRY_USERPC=`who am i | sed 's/.*(\([^]]*\)).*/\1/g'`
fi

# set some variables needed for --userpc and web plotting
# set HARRY_REMOTE_USER manually if you have a different user name on the web plot server
if [ -z $HARRY_REMOTE_USER ]; then
	export HARRY_REMOTE_USER=$USER
fi

if [[ -z ${HARRY_SSHPC} ]]; then
	export HARRY_SSHPC=$HARRY_USERPC
fi

if ! grep -q harry_sshpc $HOME/.ssh/config; then
    echo -e "" >> $HOME/.ssh/config
    echo -e "Host harry_sshpc" >> $HOME/.ssh/config
    echo -e "\tHostname = lxplus.cern.ch" >> $HOME/.ssh/config
    echo -e "\tCompression = yes" >> $HOME/.ssh/config
    echo -e "\tUser = ${HARRY_REMOTE_USER}" >> $HOME/.ssh/config
    echo -e "\tGSSAPITrustDns = yes" >> $HOME/.ssh/config
    echo -e "\tGSSAPIAuthentication = yes" >> $HOME/.ssh/config
    echo -e "\tGSSAPIDelegateCredentials = yes" >> $HOME/.ssh/config
fi

export HARRY_URL=http://${HARRY_REMOTE_USER}.web.cern.ch/${HARRY_REMOTE_USER}/plots_archive
export WEB_PLOTTING_MKDIR_COMMAND="ssh harry_sshpc mkdir -p /eos/user/${HARRY_REMOTE_USER:0:1}/${HARRY_REMOTE_USER}/www/plots_archive/{subdir}"
if [[ -z ${WEB_PLOTTING_MKDIR_COMMAND} ]]; then
    export WEB_PLOTTING_MKDIR_COMMAND="ssh harry_sshpc mkdir -p /eos/user/${HARRY_REMOTE_USER:0:1}/${HARRY_REMOTE_USER}/www/plots_archive/{subdir}"
fi

if [[ -z ${WEB_PLOTTING_COPY_COMMAND} ]]; then
    export WEB_PLOTTING_COPY_COMMAND="rsync {source} harry_sshpc:/eos/user/${HARRY_REMOTE_USER:0:1}/${HARRY_REMOTE_USER}/www/plots_archive/{subdir}"
fi

if [[ `hostname` == *naf* ]]; then
	export HP_WORK_BASE="/nfs/dust/cms/user/${USER}/Harry"
	export HP_WORK_BASE_COMMON="/nfs/dust/cms/group/higgs-kit/hp"
elif [[ `hostname` == *ekp* ]]; then
	export HP_WORK_BASE="/storage/a/${USER}/Harry"
	export HP_WORK_BASE_COMMON="/storage/a/${USER}/Harry" # no common directory yet
elif [[ `hostname` == *rwth* ]]; then
	export HP_WORK_BASE="/net/scratch_cms3b/${USER}/Harry"
	export HP_WORK_BASE_COMMON="/net/scratch_cms3b/analysis/hp"
elif [[ `hostname` == *cern* ]]; then
	export HP_WORK_BASE="/afs/cern.ch/work/${USER:0:1}/${USER}/Harry"
	export HP_WORK_BASE_COMMON="/afs/cern.ch/work/${USER:0:1}/${USER}/Harry" # no common directory yet
fi

web_plotting_no_passwd() {
	kinit ${HARRY_REMOTE_USER}@CERN.CH
}

