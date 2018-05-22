#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os
import shlex
import sys

import Artus.Utility.tools as tools


def run_command(command):
	log.debug(command)
	return logger.subprocessCall(shlex.split(command))


def main():
	
	parser = argparse.ArgumentParser(description="Run multiple commands in parallel.", parents=[logger.loggingParser])

	parser.add_argument("commands", help="Commands to be executed on a batch system. They can also be piped into this program.", nargs="*", default=[])
	parser.add_argument("-n", "--n-processes", type=int, default=1,
	                    help="Number of (parallel) processes. [Default: %(default)s]")

	args = parser.parse_args()
	logger.initLogger(args)
	
	# prepare commands
	if (len(args.commands) == 0) and (not sys.stdin.isatty()):
		args.commands.extend(sys.stdin.read().strip().split("\n"))
	
	tools.parallelize(run_command, args.commands, n_processes=args.n_processes, description=os.path.basename(sys.argv[0]))


if __name__ == "__main__":
	main()

