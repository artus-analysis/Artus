#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import numpy
import os

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError

import Artus.Utility.tfilecontextmanager as tfilecontextmanager
import Artus.Utility.tools as tools


def csv2root(args):
	csv_filename = args[0]
	variable_list = args[1]
	root_filename = os.path.splitext(csv_filename)[0]+".root"
	with tfilecontextmanager.TFileContextManager(root_filename, "RECREATE") as root_file:
		tree = ROOT.TTree("tree", csv_filename)
		tree.ReadFile(csv_filename, variable_list)
		tree.Write(tree.GetName())
	log.info("Converted {csv} to {root}:tree.".format(csv=csv_filename, root=root_filename))


def main():
	
	parser = argparse.ArgumentParser(description="Convert CSV files to ROOT files.",
	                                 parents=[logger.loggingParser])
	
	parser.add_argument("files", nargs="+",
	                    help="CSV Files.")
	parser.add_argument("--variable-lists", nargs="+", default=[""],
	                    help="Variable lists (in case the CSV has no header), e.g. var1:var2:... [Default: %(default)s]")
	parser.add_argument("-n", "--n-processes", type=int, default=1,
	                    help="Number of (parallel) processes. [Default: %(default)s]")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	if len(args.variable_lists) == 1:
		args.variable_lists = args.variable_lists * len(args.files)
	
	tools.parallelize(csv2root, zip(args.files, args.variable_lists), n_processes=args.n_processes, description="Converting")

if __name__ == "__main__":
	main()

