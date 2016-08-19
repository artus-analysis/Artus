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
	
	parser = argparse.ArgumentParser(description="Print content of ROOT file", parents=[logger.loggingParser])

	parser.add_argument("root_file", help="Input ROOT file")

	args = parser.parse_args()
	logger.initLogger(args)
	
	with TFileContextManager(args.root_file, "READ") as root_file:
		elements = roottools.RootTools.walk_root_directory(root_file)
		for index, (key, path) in enumerate(elements):
			class_name = key.GetClassName()
			log.info("%s (%s)" % (path, class_name))
			if log.isEnabledFor(logging.DEBUG):
				root_object = root_file.Get(path)
				log.debug("")
				root_object.Print("")
				if index < len(elements)-1:
					log.debug("\n" + (100*"=") + "\n")
				"""
				if (class_name == "TTree") or (class_name == "TNtuple"):
					log.debug("%s (%s, entries=%d)" % (path, class_name, root_object.GetEntries()))
				elif class_name.startswith("TH") or class_name.startswith("TProfile"):
					log.debug("%s (%s, integral=%f)" % (path, class_name, root_object.Integral()))
				elif class_name.startswith("TGraph"):
					log.debug("%s (%s, points=%d)" % (path, class_name, root_object.GetN()))
				"""

