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

import Artus.HarryPlotter.utility.roottools as roottools


if __name__ == "__main__":
	
	parser = argparse.ArgumentParser(description="Print content of ROOT file", parents=[logger.loggingParser])

	parser.add_argument("root_file", help="Input ROOT file")

	args = parser.parse_args()
	logger.initLogger(args)
	
	root_file = ROOT.TFile(args.root_file, "READ")
	elements = roottools.RootTools.walk_root_directory(root_file)
	for key, path in elements:
		log.info("%s (%s)" % (path, key.GetClassName()))

