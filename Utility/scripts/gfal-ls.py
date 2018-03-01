#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse

import Artus.Utility.dcachetools as dcachetools


def main():
	
	parser = argparse.ArgumentParser(description="Wrapper for gfal-ls supporting wildcards and recursion. Unknown arguments will be forwarded to gfal-ls", parents=[logger.loggingParser])
	
	parser.add_argument("files", nargs="+",
	                    help="Files.")
	parser.add_argument("-r", "--recursive", default=False, action="store_true",
	                    help="Recursion.")
	
	args, unknown_args = parser.parse_known_args()
	logger.initLogger(args)
	
	files = []
	for path in args.files:
		files.extend(dcachetools.list_of_files(path, args.recursive, " ".join(unknown_args)))
	for path in files:
		log.info(path)


if __name__ == "__main__":
	main()

