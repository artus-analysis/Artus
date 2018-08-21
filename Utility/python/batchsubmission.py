
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import datetime
import os
import shlex
import string
import sys
import tempfile


def batch_submission(commands, batch="rwthcondor"):
	project_directory = tempfile.mkdtemp(prefix="batch_submission_"+datetime.datetime.now().strftime("%Y-%m-%d_%H-%M")+"_")
	
	# prepare commands
	if (len(commands) == 0) and (not sys.stdin.isatty()):
		commands.extend(sys.stdin.read().strip().split("\n"))
	
	template_execute_command_n = ""
	with open(os.path.expandvars("$CMSSW_BASE/src/Artus/Utility/data/template_execute_command_n.sh"), "r") as template_execute_command_n_file:
		template_execute_command_n = template_execute_command_n_file.read().strip()
	
	execute_command_n_filename = os.path.join(project_directory, "execute_command_n.sh")
	with open(execute_command_n_filename, "w") as execute_command_n_file:
		execute_command_n_file.write(
				string.Template(template_execute_command_n).safe_substitute(
						commands="'"+("'\n\t'".join(commands))+"'"
				)
		)
	
	# prepare GC
	main_config = ""
	with open(os.path.expandvars("$CMSSW_BASE/src/Artus/Utility/data/grid-control_base_config.conf"), "r") as main_config_file:
		main_config = main_config_file.read()
	
	backend_config = ""
	with open(os.path.expandvars("$CMSSW_BASE/src/Artus/Configuration/data/grid-control_backend_" + batch + ".conf"), "r") as backend_config_file:
		backend_config = backend_config_file.read()
	
	final_config_filename = os.path.join(project_directory, "grid-control.conf")
	with open(final_config_filename, "w") as final_config_file:
		final_config_file.write(
				string.Template(main_config).safe_substitute(
						command_indices=" ".join(str(index) for index in range(len(commands))),
						backend=backend_config
				)
		)
	
	# run
	command = "go.py " + final_config_filename
	log.info(command)
	logger.subprocessCall(shlex.split(command))

