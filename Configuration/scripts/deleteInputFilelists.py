#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import shlex
import os
import subprocess


filename_replacements = {
	"root://dcache-cms-xrootd.desy.de:1094//store/user" : "srm://dcache-se-cms.desy.de:8443/srm/managerv2?SFN=/pnfs/desy.de/cms/tier2/store/user",
	"root://grid-vo-cms.physik.rwth-aachen.de:1094//store/user/" : "srm://grid-srm.physik.rwth-aachen.de:8443/srm/managerv2?SFN=/pnfs/physik.rwth-aachen.de/cms/store/user/",
	"root://grid-cms-xrootd.physik.rwth-aachen.de:1094//store/user/" : "srm://grid-srm.physik.rwth-aachen.de:8443/srm/managerv2?SFN=/pnfs/physik.rwth-aachen.de/cms/store/user/"
}


def delete_filelist(filelist):
	if not filelist.endswith(".root"):
		content = ""
		with open(filelist) as filelist_file:
			content = filelist_file.read()

		for line in content.split("\n"):
			delete_filelist(line)

	if filelist.endswith(".root"):
		for src, dst in filename_replacements.iteritems():
			filelist = filelist.replace(src, dst)
		# command = "gfal-rm " + filelist
		command = "srmrm " + filelist
		# print(command)
		log.debug(command)
		logger.subprocessCall(shlex.split(command))
	else:
		command = "git rm " + filelist
		if os.path.islink(filelist):
			command += (" " + os.path.relpath(os.path.realpath(filelist), os.getcwd()))
		log.debug(command)
		exit_code = logger.subprocessCall(shlex.split(command))
		if exit_code != 0:
			log.warning("Please execute the script in the repository, that contains the filelists!")
			logger.subprocessCall(shlex.split(command[4:]))

def main():

	parser = argparse.ArgumentParser(description="Delete filelists and their files. This script also deletes the filelists from the git repositories, if you execute the script inside the repository. Commit and push the changes if wanted afterwards.",
	                                 parents=[logger.loggingParser])
	parser.add_argument("filelists", nargs="+", help="Filelists.")
	args = parser.parse_args()
	logger.initLogger(args)

	for filelist in args.filelists:
		delete_filelist(filelist)

if __name__ == "__main__":
	main()

