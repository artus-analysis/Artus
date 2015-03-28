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

def easy_binning(binning):
	if len(set([str(upper-lower) for lower, upper in zip(binning[:-1], binning[1:])])) == 1:
		return "({n_bins}: [{lower}, {upper}])".format(
				n_bins=len(binning)-1,
				lower=binning[0],
				upper=binning[-1]
		)
	else:
		return "({n_bins}: {binning})".format(
				n_bins=len(binning)-1,
				binning=str(list(binning))
		)


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
						easy_binning(roottools.RootTools.get_binning(histogram, 0))
				))
			elif histogram.GetDimension() == 2:
				log.info("%s: %s x %s" % (
						path,
						easy_binning(roottools.RootTools.get_binning(histogram, 0)),
						easy_binning(roottools.RootTools.get_binning(histogram, 1))
				))
			else:
				log.info("%s: %s x %s x %s" % (
						path,
						easy_binning(roottools.RootTools.get_binning(histogram, 0)),
						easy_binning(roottools.RootTools.get_binning(histogram, 1)),
						easy_binning(roottools.RootTools.get_binning(histogram, 2))
				))
