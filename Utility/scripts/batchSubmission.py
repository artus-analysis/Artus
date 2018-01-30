#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import datetime
import os
import shlex
import string
import sys
import tempfile



def main():
	
	parser = argparse.ArgumentParser(description="Batch submission of multiple commands.", parents=[logger.loggingParser])

	parser.add_argument("commands", help="Commands to be executed on a batch system. They can also be piped into this program.", nargs="*", default=[])
	parser.add_argument("-b", "--batch", default="rwthcondor",
	                    help="Run with grid-control and select backend. [Default: %(default)s]")

	args = parser.parse_args()
	logger.initLogger(args)
	
	# prepare commands
	if (len(args.commands) == 0) and (not sys.stdin.isatty()):
		args.commands.extend(sys.stdin.read().strip().split("\n"))
	
	# prepare GC
	workdir = tempfile.mkdtemp(prefix="batch_submission_"+datetime.datetime.now().strftime("%Y-%m-%d_%H-%M")+"_")
	
	main_config = ""
	with open(os.path.expandvars("$CMSSW_BASE/src/Artus/Utility/data/grid-control_base_config.conf"), "r") as main_config_file:
		main_config = main_config_file.read()
	
	backend_config = ""
	with open(os.path.expandvars("$CMSSW_BASE/src/Artus/Configuration/data/grid-control_backend_" + args.batch + ".conf"), "r") as backend_config_file:
		backend_config = backend_config_file.read()
	
	final_config = string.Template(main_config).safe_substitute(
			commands="\n\t'"+("'\n\t'".join(args.commands))+"'",
			workdir=workdir,
			backend=backend_config
	)
	final_config_filename = workdir+".conf"
	with open(final_config_filename, "w") as final_config_file:
		final_config_file.write(final_config)
	
	# run
	command = "go.py " + final_config_filename
	log.info(command)
	logger.subprocessCall(shlex.split(command))
	

if __name__ == "__main__":
	main()

