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
	
	parser = argparse.ArgumentParser(description="Print out the available LHE weights in a kappa skim (see also https://lhapdf.hepforge.org/pdfsets.html).", parents=[logger.loggingParser])
	parser.add_argument("file", help="Kappa skim output file")
	args = parser.parse_args()
	logger.initLogger(args)
	
	runs = ROOT.TChain("Runs")
	runs.Add(args.file)
	runs.GetEntry(0)
	
	lheWeightNamesMap = runs.runInfo.lheWeightNamesMap
	
	mapping = []
	for lheWeightNames in lheWeightNamesMap:
		mapping.append([lheWeightNames[0], lheWeightNames[1]])
		
	log.info("\nNames of available LHE weights (human readable -> index):")
	for item in sorted(mapping, key=lambda item: item[0]):
		log.info("\t" + item[0] + " -> " + item[1])
	
	log.info("\nNames of available LHE weights (index -> human readable):")
	for item in sorted(mapping, key=lambda item: int(item[1])):
		log.info("\t" + item[1] + " -> " + item[0])
if __name__ == "__main__":
	main()

