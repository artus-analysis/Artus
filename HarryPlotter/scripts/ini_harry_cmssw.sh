#!/bin/bash

# This line fixes a bug in CMSSW6. The dateutils are not present there. This uses the dateutils from python 2.6
export PYTHONPATH=$PYTHONPATH:${CMSSW_RELEASE_BASE}/../../../../slc5_amd64_gcc462/external/py2-matplotlib/1.0.1/lib/python2.6/site-packages/

# set some variables needed for --userpc and web plotting
# set HARRY_REMOTE_USER manually if you have a different user name on the web plot server
if [ -z $HARRY_REMOTE_USER ]; then
	export HARRY_REMOTE_USER=$USER
fi
export HARRY_USERPC=`who am i | sed 's/.*(\([^]]*\)).*/\1/g'`

#if not logged in from ekplx, fall back to lx26
if [[ $HARRY_USERPC == *"ekplx"* ]]; then
	export HARRY_SSHPC=$HARRY_USERPC
else
	export HARRY_SSHPC=ekplx26.physik.uni-karlsruhe.de
fi
export HARRY_REMOTE_DIR=plots_archive
export HARRY_REMOTE_PATH=/disks/ekpwww/web/${HARRY_REMOTE_USER}/public_html
export HARRY_URL=http://www-ekp.physik.uni-karlsruhe.de/~${HARRY_REMOTE_USER}
