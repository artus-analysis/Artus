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
from Artus.Utility.tfilecontextmanager import TFileContextManager

if __name__ == "__main__":
	
	parser = argparse.ArgumentParser(description="Print numbers of entries of trees.", parents=[logger.loggingParser])

	parser.add_argument("root_file", help="Input ROOT file")

	args = parser.parse_args()
	logger.initLogger(args)
	
	with TFileContextManager(args.root_file, "READ") as root_file:
		elements = roottools.RootTools.walk_root_directory(root_file)
	
		for key, path in elements:
			if key.GetClassName().startswith("TTree") or key.GetClassName().startswith("TNtuple"):
				tree = root_file.Get(path)
				log.info("%s: %d entries" % (path, tree.GetEntries()))
			elif key.GetClassName().startswith("TH") or key.GetClassName().startswith("TProfile"):
				histogram = root_file.Get(path)
				log.info("%s: %d entries, integral %f" % (path, histogram.GetEntries(), histogram.Integral()))

