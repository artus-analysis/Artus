#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)
import argparse
import os

# Usage example:
# make_all_2d.py -x alpha beta gamma -b "41,0,5" "81,0,200" "41,-3.2,3.2" -o '-i rootfile.root -f ntuple -o outputname -w "(weight1)*(weight2)" --analysis-modules PrintInfos'

parser = argparse.ArgumentParser(description="Create 2D histograms for all combinations of given branches.")
parser.add_argument("-x", "--x-expressions", nargs="+", help="List of all branches. (First entry will always be on x-axis and last entry on y-axis)")
parser.add_argument("-b", "--x-bins", nargs="+", help="Binning for each branch. (The order corrensponds to the branches given by '-x')")
parser.add_argument("-o", "--other", help="All other commands given to harry.py.")
args = parser.parse_args()
logger.initLogger(args)

if len(args.x_expressions) != len(args.x_bins):
	log.info("Number of branches doesn't correspond to number of given binnings! Exiting...")
 	exit(0)

for i,(exprone,binone) in enumerate(zip(args.x_expressions,args.x_bins)):
	for j,(exprtwo,bintwo) in enumerate(zip(args.x_expressions,args.x_bins)):
		if j <= i: continue
		command = "harry.py " + args.other + " -x " + exprone + " -y " + exprtwo + " --x-bins " + binone + " --y-bins " + bintwo
		log.info("\n>> " + command)
		os.system(command)


log.info("\nDone!")
exit(0)
