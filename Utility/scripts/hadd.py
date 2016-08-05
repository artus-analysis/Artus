#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse

import Artus.Utility.jsonTools as jsonTools
import Artus.Utility.tools as tools

import glob
import os
import shlex
import sys
from Artus.Utility.tools import hadd


def main():
	
	parser = argparse.ArgumentParser(description="Wrapper for hadd avoiding too many bash arguments.", parents=[logger.loggingParser])
	
	parser.add_argument("-t", "--target-file", required=True,
	                    help="Target file.")
	parser.add_argument("source_files", nargs="+",
	                    help="Source file. Can be either separate files or lists of files separated by whitespaces in one or more arguments.")
	parser.add_argument("-a", "--args", default="",
	                    help="Options for hadd. [Default: %(default)s]")
	parser.add_argument("-n", "--max-files", default=500, type=int,
	                    help="Maximum number of source files use per hadd call. [Default: %(default)s]")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	source_files = []
	for arg in args.source_files:
		for item in shlex.split(arg.replace("\"", "")):
			matching_files = glob.glob(item)
			if len(matching_files) > 0:
				source_files.extend(matching_files)
			else:
				source_files.append(item)
	
	sys.exit(hadd(target_file=args.target_file,
	              source_files=source_files,
	              hadd_args=args.args,
	              max_files=args.max_files))
	

if __name__ == "__main__":
	main()

