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
from Artus.HarryPlotter.utility.tfilecontextmanager import TFileContextManager

if __name__ == "__main__":
	
	parser = argparse.ArgumentParser(description="Print contents of trees.", parents=[logger.loggingParser])

	parser.add_argument("root_file", help="Input ROOT file")

	args = parser.parse_args()
	logger.initLogger(args)
	
	with TFileContextManager(args.root_file, "READ") as root_file:
		elements = roottools.RootTools.walk_root_directory(root_file)
	
		for key, path in elements:
			if key.GetClassName().startswith("TTree") or key.GetClassName().startswith("TNtuple"):
				tree = root_file.Get(path)
				log.info("\nContent of tree \"tree\":\n".format(tree=path))
				tree.Scan("*")

