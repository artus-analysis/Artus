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



def hadd(target_file, source_files, hadd_args="", max_files=500):
	if len(source_files) == 0:
		log.critical("No source files specified to be merged!")
		sys.exit(1)
	
	source_files_chunks = [source_files[start_chunk_index:start_chunk_index+max_files] for start_chunk_index in xrange(0, len(source_files), max_files)]
	
	exit_code = 0
	for chunk_index, tmp_source_files in enumerate(source_files_chunks):
		tmp_target_file = "%s.hadd_%d.root" % (target_file, chunk_index)
		if chunk_index == len(source_files_chunks)-1:
			tmp_target_file = target_file
		
		last_target_file = ""
		if chunk_index > 0:
			last_target_file = "%s.hadd_%d.root" % (target_file, chunk_index-1)
		
		command = "hadd %s %s %s %s" % (hadd_args, tmp_target_file, " ".join(tmp_source_files), last_target_file)
		log.debug(command)
		exit_code = max(exit_code, logger.subprocessCall(shlex.split(command)))
		
		# remove last temp. merge result
		if len(last_target_file) > 0:
			os.remove(last_target_file)
	return exit_code


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

