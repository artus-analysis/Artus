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
	
	parser = argparse.ArgumentParser(description="Print binning of histograms", parents=[logger.loggingParser])

	parser.add_argument("root_file", help="Input ROOT file")

	args = parser.parse_args()
	logger.initLogger(args)
	
	root_file = ROOT.TFile(args.root_file, "READ")
	elements = roottools.RootTools.walk_root_directory(root_file)
	
	for key, path in elements:
		if key.GetClassName().startswith("TH"):
			histogram = root_file.Get(path)
			if histogram.GetDimension() == 1:
				log.info("%s: %s" % (
						path,
						roottools.RootTools.binning_formatted(roottools.RootTools.get_binning(histogram, 0))
				))
			elif histogram.GetDimension() == 2:
				log.info("%s: %s x %s" % (
						path,
						roottools.RootTools.binning_formatted(roottools.RootTools.get_binning(histogram, 0)),
						roottools.RootTools.binning_formatted(roottools.RootTools.get_binning(histogram, 1))
				))
			else:
				log.info("%s: %s x %s x %s" % (
						path,
						roottools.RootTools.binning_formatted(roottools.RootTools.get_binning(histogram, 0)),
						roottools.RootTools.binning_formatted(roottools.RootTools.get_binning(histogram, 1)),
						roottools.RootTools.binning_formatted(roottools.RootTools.get_binning(histogram, 2))
				))
