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

export HARRY_URL=http://${HARRY_REMOTE_USER}.web.cern.ch/${HARRY_REMOTE_USER}/plots_archive

if [[ -z ${WEB_PLOTTING_MKDIR_COMMAND} ]]; then
	export WEB_PLOTTING_MKDIR_COMMAND="xrdfs eosuser.cern.ch mkdir -p /eos/user/${HARRY_REMOTE_USER:0:1}/${HARRY_REMOTE_USER}/www/plots_archive/{subdir}"
fi

if [[ -z ${WEB_PLOTTING_COPY_COMMAND} ]]; then
	export WEB_PLOTTING_COPY_COMMAND="xrdcp -s -f {source} root://eosuser.cern.ch//eos/user/${HARRY_REMOTE_USER:0:1}/${HARRY_REMOTE_USER}/www/plots_archive/{subdir}"
fi

start_ssh_agent() {
	echo "Initialising new SSH agent..."
	/usr/bin/ssh-agent | sed 's/^echo/#echo/' > "${SSH_ENV}"
	echo succeeded
	chmod 600 "${SSH_ENV}"
	. "${SSH_ENV}" > /dev/null
	/usr/bin/ssh-add;
}

web_plotting_no_passwd() {
	kinit ${HARRY_REMOTE_USER}@CERN.CH
}

