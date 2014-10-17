#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import json
import pprint
import sys
import tempfile

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

import Artus.Configuration.jsonTools as jsonTools



def main():
	ROOT.gROOT.SetBatch(True)
	
	parser = argparse.ArgumentParser(description="Compare or print Artus configurations", parents=[logger.loggingParser])

	configGroup = parser.add_mutually_exclusive_group()
	configGroup.add_argument("-c", "--compare", help="Use compare mode. Needs two files. [Default in case of two input files]", action="store_true", default=True)
	configGroup.add_argument("-p", "--print", help="Use print mode. Needs one file. [Default in case of one input file]", action="store_false", dest="compare")
	
	parser.add_argument("-1", "--pipelines-1", help="Pipeline names of first file to be compared or printed. [Default: complete config]", nargs="*", default=[None])
	parser.add_argument("-2", "--pipelines-2", help="Pipeline names of second file to be compared. [Default: complete config]", nargs="*", default=[None])
	
	parser.add_argument("files", help="One or two configurations. Can be either Artus output root files or JSON text files", nargs="+")

	args = parser.parse_args()
	logger.initLogger(args)
	
	if args.compare and len(args.files) < 2: args.compare = False
	
	if args.compare:
		for pipeline1 in args.pipelines_1:
			dictA = jsonTools.JsonDict(args.files[0])
			if pipeline1 != None:
				dictA = dictA.get("Pipelines", {}).get(pipeline1, {})
			
			dictA_filename = tempfile.mktemp(prefix="artusConfigDiff_")
			with open(dictA_filename, "w") as dictA_file:
				dictA_file.write(jsonTools.JsonDict(dictA).toString())
				
			for pipeline2 in args.pipelines_2:
				dictB = jsonTools.JsonDict(args.files[1])
				if pipeline2 != None:
					dictB = dictB.get("Pipelines", {}).get(pipeline2, {})
			
				dictB_filename = tempfile.mktemp(prefix="artusConfigDiff_")
				with open(dictB_filename, "w") as dictB_file:
					dictB_file.write(jsonTools.JsonDict(dictB).toString())
				
				diffDictA, diffDictB = jsonTools.JsonDict(dictA) - jsonTools.JsonDict(dictB)
		
				log.info("Diff in file \"%s\"%s:\n" % (args.files[0], ("" if pipeline1 == None else (" (Pipeline \"%s\")" % pipeline1))))
				log.info(jsonTools.JsonDict(diffDictA))
		
				log.info("\n\n")
		
				log.info("Diff in file \"%s\"%s:\n" % (args.files[1], ("" if pipeline2 == None else (" (Pipeline \"%s\")" % pipeline2))))
				log.info(jsonTools.JsonDict(diffDictB))
				
				log.info("\nmeld %s %s" % (dictA_filename, dictB_filename))
				
				log.info((50*"-") + "\n")
	
	else:
		print args.files
		print args.pipelines_1
	
		for pipeline1 in args.pipelines_1:
			dictA = jsonTools.JsonDict(args.files[0])
			if pipeline1 != None:
				dictA = dictA.get("Pipelines", {}).get(pipeline1, {})
			
			log.info("Config in file \"%s\"%s:\n" % (args.files[0], ("" if pipeline1 == None else (" (Pipeline \"%s\")" % pipeline1))))
			log.info(jsonTools.JsonDict(dictA))
			
			log.info((50*"-") + "\n")


if __name__ == "__main__":
	main()

