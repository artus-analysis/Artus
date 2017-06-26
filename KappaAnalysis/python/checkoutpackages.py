# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import os
import sys


def checkout_packages(max_n_trials=2):
	commands = [
		"cd $CMSSW_BASE/src",
		
		# needed by the JetEnergyCorrectionProducer
		"git cms-addpkg CondFormats/JetMETObjects",
	]
	execute_commands(commands, max_n_trials=max_n_trials)


def execute_commands(commands, max_n_trials=2):
	for command in commands:
		log.info("\nExecute \"%s\"." % command)
		exit_code = 1
		n_trials = 0
		while exit_code != 0:
			n_trials += 1
			print n_trials, max_n_trials
			if n_trials > max_n_trials:
				log.error("Last command could not be executed successfully! Stop program!")
				sys.exit(1)

			if command.startswith("cd"):
				os.chdir(os.path.expandvars(command.replace("cd ", "")))
				print os.path.abspath(os.getcwd())
				print os.path.abspath(os.path.expandvars(command.replace("cd ", "")))
				print (os.path.abspath(os.path.expandvars(command.replace("cd ", ""))) != os.path.abspath(os.getcwd()))
				print int(os.path.abspath(os.path.expandvars(command.replace("cd ", ""))) != os.path.abspath(os.getcwd()))
				exit_code = int((os.path.abspath(os.path.expandvars(command.replace("cd ", ""))) != os.path.abspath(os.getcwd())))
			else:
				exit_code = logger.subprocessCall(command.split())

