#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import json
import logging
import pprint
import sys

import Artus.Configuration.logger as logger
import Artus.Configuration.jsonTools as jsonTools


def main():
	parser = argparse.ArgumentParser(description="Compare or print Artus configurations", parents=[logger.loggingParser])

	configGroup = parser.add_mutually_exclusive_group()
	configGroup.add_argument("-c", "--compare", help="Use compare mode. [Default]", action="store_true", default=True)
	configGroup.add_argument("-p", "--print", help="Use print mode.", action="store_false", dest="compare")
	
	parser.add_argument("files", help="One or two configurations. Can be either Artus output root files or JSON text files", nargs="+")

	args = parser.parse_args()
	logger.initLogger(args)
	
	pathInRootFile = "config"
	indent = 4
	
	if args.compare and len(args.files) < 2:
		logging.getLogger(__name__).critial("Need two files to compare.")
		sys.exit(1)
	if args.compare and len(args.files) > 2:
		logging.getLogger(__name__).warning("Only the first two files can be compared.")
	
	if args.compare:
		diffDictA, diffDictB = jsonTools.deepdiff(jsonTools.readJsonDict(args.files[0], pathInRootFile),
		                                          jsonTools.readJsonDict(args.files[1], pathInRootFile))
		
		logging.getLogger(__name__).info( "Diff in file \"%s\":\n" % args.files[0])
		jsonTools.printJsonDict(diffDictA)
		
		logging.getLogger(__name__).info("\n\n")
		
		logging.getLogger(__name__).info("Diff in file \"%s\":\n" % args.files[1])
		jsonTools.printJsonDict(diffDictB)
	
	else:
		jsonTools.printJsonDict(jsonTools.readJsonDict(args.files[0], pathInRootFile))


if __name__ == "__main__":
	main()

