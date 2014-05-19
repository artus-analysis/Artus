#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os

import ROOT


def main():
	ROOT.gROOT.SetBatch(True)
	ROOT.gSystem.Load(os.path.expandvars("$CMSSW_BASE/src/Kappa/lib/libKappa"))
	
	parser = argparse.ArgumentParser(description="Print out the available tau discriminators in a kappa skim.", parents=[logger.loggingParser])
	parser.add_argument("file", help="Kappa skim output file containing the Tau Meta data")
	args = parser.parse_args()
	logger.initLogger(args)
	
	root_file = ROOT.TFile(args.file, "READ")
	lumis = root_file.Get("Lumis")
	lumis.GetEntry(0)
	
	log.info("\nNames of available binary discriminators:")
	binaryDiscriminators = lumis.hpsPFTaus.binaryDiscriminatorNames
	for binaryDiscriminator in binaryDiscriminators:
		log.info("\t" + binaryDiscriminator)
	
	log.info("\nNames of available floating-point-type discriminators:")
	floatDiscriminators = lumis.hpsPFTaus.floatDiscriminatorNames
	for floatDiscriminator in floatDiscriminators:
		log.info("\t" + floatDiscriminator)


if __name__ == "__main__":
	main()

