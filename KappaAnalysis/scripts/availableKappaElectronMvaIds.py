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
	
	parser = argparse.ArgumentParser(description="Print out the available electron MVA IDs in a kappa skim.", parents=[logger.loggingParser])
	parser.add_argument("file", help="Kappa skim output file containing the electron meta data")
	args = parser.parse_args()
	logger.initLogger(args)
	
	lumis = ROOT.TChain("Lumis")
	lumis.Add(args.file)
	lumis.GetEntry(0)
	
	log.info("\nNames of available electron MVA IDs:")
	idNames = lumis.electronMetadata.idNames
	for idName in idNames:
		log.info("\t" + idName)


if __name__ == "__main__":
	main()

