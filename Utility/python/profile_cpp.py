#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import tempfile
import os


def profile_cpp(command, profiler, profiler_opt, output_dir=None):

	log.warning("Did you compile your code with debug flags? scram b USER_CXXFLAGS=\"-g\"")

	if not isinstance(command, basestring):
		command = " ".join(command)

	if output_dir is None:
		output_dir = tempfile.mkdtemp(prefix="profile_cpp_")

	profile_commands = None
	profile_outputs = None

	if profiler == "igprof":
		if profiler_opt == "pp":
			# call C++ executable with profiler igprof
			profile_outputs = [os.path.join(output_dir, filename) for filename in ["igprof.pp.gz", "igprof.analyse.txt", "igprof.analyse.sql3"]]

			profile_commands = [
			"igprof -d -pp -z -o " + profile_outputs[0] + " " + command,
			"execute-command.sh igprof-analyse -d -v -g " + profile_outputs[0] + " > " + profile_outputs[1],
			"execute-command.sh igprof-analyse --sqlite -d -v -g " + profile_outputs[0] + " | sqlite3 " + profile_outputs[2],
			"igprof-navigator " + profile_outputs[2],
			]
		elif profiler_opt == "mp":
			# call C++ executable with profiler igprof
			profile_outputs = [os.path.join(output_dir, filename) for filename in ["igprof.mp.gz", "igprof.analyse.txt", "igprof.analyse.sql3"]]

			profile_commands = [
			"igprof -d -mp -z -o " + profile_outputs[0] + " " + command,
			"execute-command.sh igprof-analyse -d -v -g -r MEM_TOTAL " + profile_outputs[0] + " > " + profile_outputs[1],
			"execute-command.sh igprof-analyse --sqlite -d -v -g -r MEM_TOTAL " + profile_outputs[0] + " | sqlite3 " + profile_outputs[2],
			"igprof-navigator " + profile_outputs[2],
			]
		else:
			log.info(profiler_opt + "is not a valid option")

	elif profiler == "valgrind":
		if profiler_opt == "pp":
			# call C++ executable with profiler valgrind
			profile_outputs = [os.path.join(output_dir, filename) for filename in ["valgrind.callgrind.out"]]

			profile_commands = [
			"valgrind --tool=callgrind --callgrind-out-file=" + profile_outputs[0] + " " + command,
			"callgrind_annotate --auto=yes " + profile_outputs[0]
			]
		elif profiler_opt == "mp":
			# call C++ executable with profiler valgrind
			profile_outputs = [os.path.join(output_dir, filename) for filename in ["valgrind.massif.out", "valgrind.massif.txt"]]

			profile_commands = [
			"valgrind --tool=massif --massif-out-file=" + profile_outputs[0] + " " + command,
			"ms_print " + profile_outputs[0] + " > " + profile_outputs[1]
			]
		else:
			log.info(profiler_opt + "is not a valid option")

	else:
		log.info(profiler + "is not a valid profiler")

	for command in profile_commands:
		log.info("Execute \"%s\"." % command)
		logger.subprocessCall(command.split())

	if profile_outputs:
		log.info("Profiling output is written to \"%s\"." % "\", \"".join(profile_outputs))

