#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError


def main():
	
	
	parser = argparse.ArgumentParser(description="Print out the available jet taggers in a kappa skim.", parents=[logger.loggingParser])
	parser.add_argument("file", help="Kappa skim output file containing the tagged jet meta data")
	args = parser.parse_args()
	logger.initLogger(args)
	
	root_file = ROOT.TFile(args.file, "READ")
	lumis = root_file.Get("Lumis")
	lumis.GetEntry(0)
	
	log.info("\nNames of available jet taggers:")
	tagNames = lumis.jetMetadata.tagNames
	for tagName in tagNames:
		log.info("\t" + tagName)
	
	log.info("\nNames of available jet IDs:")
	idNames = lumis.jetMetadata.idNames
	for idName in idNames:
		log.info("\t" + idName)


if __name__ == "__main__":
	main()

