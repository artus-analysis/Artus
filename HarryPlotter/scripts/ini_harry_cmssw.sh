#!/bin/bash

# This line fixes a bug in CMSSW6. The dateutils are not present there. This uses the dateutils from python 2.6
export PYTHONPATH=$PYTHONPATH:${CMSSW_RELEASE_BASE}/../../../../slc5_amd64_gcc462/external/py2-matplotlib/1.0.1/lib/python2.6/site-packages/

# set some variables needed for --userpc and web plotting
# set HARRY_REMOTE_USER manually if you have a different user name on the web plot server
if [ -z $HARRY_REMOTE_USER ]; then
	export HARRY_REMOTE_USER=$USER
fi

if [[ -z ${HARRY_USERPC} ]]; then
	export HARRY_USERPC=`who am i | sed 's/.*(\([^]]*\)).*/\1/g'`
fi

if [[ -z ${HARRY_SSHPC} ]]; then
	#if logged in from an ekplx host, assume it is reachable
	if [[ $HARRY_USERPC == *"ekplx"* ]]; then
		export HARRY_SSHPC=$HARRY_USERPC
	else
		# find a reachable host - try ekplxs in different offices
		for lxid in 26 30 69 77 14 17 21 25; do
			if ping -c 1 ekplx${lxid}.physik.uni-karlsruhe.de 2>/dev/null 1>&2; then
				export HARRY_SSHPC=ekplx${lxid}.physik.uni-karlsruhe.de
				break
			fi
		done
	fi
	if [[ -z ${HARRY_SSHPC} ]]; then
		echo "Failed to find reachable \$HARRY_SSHP host" 1>&2
	fi
fi

export HARRY_REMOTE_DIR=plots_archive
export HARRY_REMOTE_PATH=/disks/ekpwww/web/${HARRY_REMOTE_USER}/public_html
export HARRY_URL=http://www.ekp.kit.edu/~${HARRY_REMOTE_USER}
