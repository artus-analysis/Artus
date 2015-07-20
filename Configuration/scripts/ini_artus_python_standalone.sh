#!/bin/bash


ini_python_standalone() {
	clean_python_setup $@
	
	for REPO_DIR in $@
	do
		export REPO_DIR=`readlink -e ${REPO_DIR}`
	
		# create symlinks for python directories
		rm -rf ${REPO_DIR}/.python_links
        mkdir -p ${REPO_DIR}/.python_links
        for PYTHON_DIR in `find ${REPO_DIR} -name python -type d`
        do
        	export REL_PYTHON_DIR=$(echo ${PYTHON_DIR} | sed -e "s@`dirname ${REPO_DIR}`@@g" -e "s@^/@@g")
        	mkdir -p ${REPO_DIR}/.python_links/`dirname ${REL_PYTHON_DIR}`
        	ln -s `readlink -e ${PYTHON_DIR}`/* ${REPO_DIR}/.python_links/`dirname ${REL_PYTHON_DIR}`/
        done
		export PYTHONPATH=${REPO_DIR}/.python_links/:${PYTHONPATH}
		
		# create empty __init__.py files
		for SUB_DIR in `find -L ${REPO_DIR}/.python_links/* -type d`
		do
			touch $SUB_DIR/__init__.py
		done
		
		# add scripts directories to $PATH
        for SCRIPTS_DIR in `find ${REPO_DIR} -name scripts -type d`
        do
        	export PATH=`readlink -e ${SCRIPTS_DIR}`:${PATH}
        done
	done
}

clean_python_setup() {
	for REPO_DIR in $@
	do
		# clean $PYTHONPATH
		export PYTHONPATH=$(echo ${PYTHONPATH} | sed -e "s@`readlink -e ${REPO_DIR}`/.python_links/:@@g")
		
		# clean $PATH
        for SCRIPTS_DIR in `find ${REPO_DIR} -name scripts -type d`
        do
			export PATH=$(echo ${PATH} | sed -e "s@`readlink -e ${SCRIPTS_DIR}`:@@g")
        done
        
        # remove links directory
		rm -rf $REPO_DIR/.python_links
	done
}

export ARTUSPATH=$(readlink -e `dirname $BASH_SOURCE`/../../)
ini_python_standalone ${ARTUSPATH}

