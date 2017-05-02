#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse

import Artus.Utility.profile_cpp as profile_cpp


def main():
	
	parser = argparse.ArgumentParser(description="Profile C++ executable with arguments.",
	                                 parents=[logger.loggingParser])
	
	parser.add_argument("command", nargs="+",
	                    help="Command to be profiled.")
	parser.add_argument("-p", "--profiler", default="igprof", choices=["igprof", "valgrind"],
	                    help="Profiling tool. [Default: %(default)s]")
	parser.add_argument("-o", "--output-dir", default=None,
	                    help="Output directory. [Default: tmp. directory]")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	profile_cpp.profile_cpp(command=args.command, profiler=args.profiler, output_dir=args.output_dir)

if __name__ == "__main__":
	main()

