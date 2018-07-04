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
parser.add_argument("-x", "--x-expressions", nargs="+", help="List of all branches")
parser.add_argument("-b", "--x-bins", nargs="+", help="Binning for each branch. (The order corrensponds to the branches given by '-x'!)")
parser.add_argument("-o", "--other", help="All other commands given to harry.py.")
args = parser.parse_args()
logger.initLogger(args)

if len(args.x_expressions) != len(args.x_bins):
  print "Number of branches doesn't correspond to number of given binnings! Exiting..."
  exit(0)

n = len(args.x_expressions)
for i in range(n):
  for j in range(n):
    if i <= j: continue
    command = "harry.py " + args.other + " -x " + args.x_expressions[i] + " -y " + args.x_expressions[j] + " --x-bins " + args.x_bins[i] + " --y-bins " + args.x_bins[j]
    print "\n>> " + command
    os.system(command)


print "\nDone!"
exit(0)
