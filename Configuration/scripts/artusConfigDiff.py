#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import json
import pprint
import sys

import ROOT

import Artus.Configuration.jsonTools as jsonTools



def main():
	ROOT.gROOT.SetBatch(True)
	
	parser = argparse.ArgumentParser(description="Compare or print Artus configurations", parents=[logger.loggingParser])

	configGroup = parser.add_mutually_exclusive_group()
	configGroup.add_argument("-c", "--compare", help="Use compare mode. Needs two files. [Default in case of two input files]", action="store_true", default=True)
	configGroup.add_argument("-P", "--compare-pipelines", help="Use compare pipelines mode. Needs one file.", action="store_true", default=False)
	configGroup.add_argument("-p", "--print", help="Use print mode. Needs one file. [Default in case of one input file]", action="store_false", dest="compare")
	
	parser.add_argument("-1", "--pipelines-1", help="First set of pipeline names to compared with the second set in compare pipelines mode", nargs="*", default=[])
	parser.add_argument("-2", "--pipelines-2", help="Second set of pipeline names to compared with the first set in compare pipelines mode", nargs="*", default=[])
	
	parser.add_argument("files", help="One or two configurations. Can be either Artus output root files or JSON text files", nargs="+")

	args = parser.parse_args()
	logger.initLogger(args)
	
	if args.compare_pipelines: args.compare = False
	if args.compare and len(args.files) < 2: args.compare = False
	
	minFiles = 1
	maxFiles = 2 if args.compare else 1
	if len(args.files) < minFiles:
		log.critical("Need a mininum number of %d input files!" % minFiles)
		sys.exit(1)
	if len(args.files) > maxFiles:
		log.warning("Only the first %d file(s) are taken as input." % maxFiles)
	
	if args.compare:
		diffDictA, diffDictB = jsonTools.JsonDict(args.files[0]) - jsonTools.JsonDict(args.files[1])
		
		log.info("Diff in file \"%s\":\n" % args.files[0])
		log.info(jsonTools.JsonDict(diffDictA))
		
		log.info("\n\n")
		
		log.info("Diff in file \"%s\":\n" % args.files[1])
		log.info(jsonTools.JsonDict(diffDictB))
	
	elif args.compare_pipelines:
		jsonDict = jsonTools.JsonDict(args.files[0])
		
		if len(args.pipelines_1) > 0 and len(args.pipelines_2) == 0:
			args.pipelines_2 = filter(lambda pipelineName: pipelineName not in args.pipelines_1, jsonDict.get("Pipelines", {}).keys())
		elif len(args.pipelines_1) == 0 and len(args.pipelines_2) > 0:
			args.pipelines_1 = filter(lambda pipelineName: pipelineName not in args.pipelines_2, jsonDict.get("Pipelines", {}).keys())
		elif len(args.pipelines_1) == 0 and len(args.pipelines_2) == 0:
			args.pipelines_1 = jsonDict.get("Pipelines", {}).keys()
			args.pipelines_2 = jsonDict.get("Pipelines", {}).keys()
		
		pipelines1, pipelines2 = {}, {}
		for pipelineName in args.pipelines_1: pipelines1[pipelineName] = jsonDict.get("Pipelines", {}).get(pipelineName, {})
		for pipelineName in args.pipelines_2: pipelines2[pipelineName] = jsonDict.get("Pipelines", {}).get(pipelineName, {})
		
		
		for pipelineName1, pipeline1 in pipelines1.items():
			for pipelineName2, pipeline2 in pipelines2.items():
				if pipelineName1 == pipelineName2: continue
				
				log.info("Comparison of pipeline \"%s\" with pipeline \"%s\":\n" % (pipelineName1, pipelineName2))
				
				diffDictA, diffDictB = jsonTools.JsonDict(pipeline1) - jsonTools.JsonDict(pipeline2)
		
				log.info("Diff in pipeline \"%s\":\n" % pipelineName1)
				log.info(jsonTools.JsonDict(diffDictA))
		
				log.info("\n\n")
		
				log.info("Diff in pipeline \"%s\":\n" % pipelineName2)
				log.info(jsonTools.JsonDict(diffDictB))
				
				log.info((50*"-") + "\n")
				
	
	else:
		print jsonTools.JsonDict(args.files[0])


if __name__ == "__main__":
	main()

