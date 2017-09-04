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
	#if logged in from an ekplx host, assume it is reachable
	if [[ $HARRY_USERPC == *"ekplx"* ]]; then
		export HARRY_SSHPC=$HARRY_USERPC
	elif [[ $HARRY_USERPC == *"rwth"* ]]; then
		export HARRY_SSHPC=$HARRY_USERPC
	fi
	if [[ -z ${HARRY_SSHPC} ]]; then
		echo "Failed to find reachable \$HARRY_SSHP host" 1>&2
	fi
fi

if [[ $HARRY_SSHPC == *"rwth"* ]]; then
	export HARRY_URL=http://${HARRY_REMOTE_USER}.web.cern.ch/${HARRY_REMOTE_USER}/plots_archive
else
	export HARRY_URL=http://www.ekp.kit.edu/~${HARRY_REMOTE_USER}/plots_archive
fi

if [[ -z ${WEB_PLOTTING_MKDIR_COMMAND} ]]; then
	if [[ $HARRY_USERPC == *"rwth"* ]]; then
		export WEB_PLOTTING_MKDIR_COMMAND="xrdfs eosuser.cern.ch mkdir -p /eos/user/${HARRY_REMOTE_USER:0:1}/${HARRY_REMOTE_USER}/www/plots_archive/{subdir}"
	else
		export WEB_PLOTTING_MKDIR_COMMAND="ssh ${HARRY_REMOTE_USER}@${HARRY_SSHPC} mkdir -p /ekpwww/web/${HARRY_REMOTE_USER}/public_html/plots_archive/{subdir}"
	fi
fi

if [[ -z ${WEB_PLOTTING_COPY_COMMAND} ]]; then
	if [[ $HARRY_USERPC == *"rwth"* ]]; then
		export WEB_PLOTTING_COPY_COMMAND="xrdcp -s -f {source} root://eosuser.cern.ch//eos/user/${HARRY_REMOTE_USER:0:1}/${HARRY_REMOTE_USER}/www/plots_archive/{subdir}"
	else
		export WEB_PLOTTING_COPY_COMMAND="rsync -u {source} ${HARRY_REMOTE_USER}@${HARRY_SSHPC}:/ekpwww/web/${HARRY_REMOTE_USER}/public_html/plots_archive/{subdir}"
	fi
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
	if [[ $HARRY_USERPC == *"rwth"* ]]; then
		kinit ${HARRY_REMOTE_USER}@CERN.CH
		aklog -c cern.ch #For web-plotting on EOS, this line isn't needed
	else
		# startup ssh-agent
		unset SSH_ASKPASS
		SSH_ENV="$HOME/.ssh/environment"
		if [ -f "${SSH_ENV}" ]; then
			. "${SSH_ENV}" > /dev/null
			ps -ef | grep ${SSH_AGENT_PID} | grep ssh-agent$ > /dev/null || start_agent
		else
			start_ssh_agent;
		fi
	fi
}

