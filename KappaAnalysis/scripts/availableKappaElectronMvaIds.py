#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError


def main():
	ROOT.gROOT.SetBatch(True)
	ROOT.gSystem.Load(os.path.expandvars("$CMSSW_BASE/src/Kappa/lib/libKappa"))
	
	parser = argparse.ArgumentParser(description="Print out the available electron MVA IDs in a kappa skim.", parents=[logger.loggingParser])
	parser.add_argument("file", help="Kappa skim output file containing the electron meta data")
	args = parser.parse_args()
	logger.initLogger(args)
	
	root_file = ROOT.TFile(args.file, "READ")
	lumis = root_file.Get("Lumis")
	lumis.GetEntry(0)
	
	log.info("\nNames of available electron MVA IDs:")
	idNames = lumis.electronMetadata.idNames
	for idName in idNames:
		log.info("\t" + idName)


if __name__ == "__main__":
	main()

