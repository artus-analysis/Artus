#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import math
import numpy
import os

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError

import Artus.Utility.progressiterator as progressiterator
import Artus.HarryPlotter.utility.tfilecontextmanager as tfilecontextmanager


def main():
	
	parser = argparse.ArgumentParser(description="Sort trees.",
	                                 parents=[logger.loggingParser])
	
	parser.add_argument("inputs", nargs="+",
	                    help="Input files containing the tree to sort.")
	parser.add_argument("-t", "--tree", required=True,
	                    help="Path to the tree object in the file.")
	parser.add_argument("-b", "--branches", nargs="+", default=["run", "lumi", "event"],
	                    help="Branch names to be considered for the sorting.")
	parser.add_argument("-o", "--output", default="output.root",
	                    help="Output ROOT file.")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	args.branches = args.branches[:4]
	
	# https://root.cern.ch/root/roottalk/roottalk01/3646.html
	
	log.info("Opening input from")
	input_tree = ROOT.TChain()
	for input_file in args.inputs:
		path = os.path.join(input_file, args.tree)
		log.info("\t"+path)
		input_tree.Add(path)
	input_tree.SetCacheSize(128*1024*1024)
	n_entries = input_tree.GetEntries()
	
	values = [[] for index in xrange(len(args.branches))]
	n_entries_per_iteration = 10000000 # larger buffers make problems
	for iteration in progressiterator.ProgressIterator(range(int(math.ceil(n_entries / float(n_entries_per_iteration)))), description="Retrieving branch values for sorting"):
		cut = "(Entry$>=({i}*{n}))*(Entry$<(({i}+1)*{n}))".format(i=iteration, n=n_entries_per_iteration)
		input_tree.Draw(":".join(args.branches), cut, "goff")
		buffers = [input_tree.GetV1(), input_tree.GetV2(), input_tree.GetV3(), input_tree.GetV4()][:len(args.branches)]
		for index, input_buffer in enumerate(buffers):
			values[index].extend(list(numpy.ndarray(input_tree.GetSelectedRows(), dtype=numpy.double, buffer=input_buffer)))
	
	log.info("Sorting of the tree entry indices...")
	values = zip(*([range(n_entries)]+values))
	values.sort(key=lambda value: value[1:])
	sorted_entries = list(zip(*values)[0])
	
	log.info("Creating output "+args.output+"...")
	with tfilecontextmanager.TFileContextManager(args.output, "RECREATE") as output_file:
		output_tree = input_tree.CloneTree(0)
		for entry in progressiterator.ProgressIterator(sorted_entries, description="Copying tree entries"):
			input_tree.GetEntry(entry)
			output_tree.Fill()
		output_file.Write()
	log.info("Save sorted tree in "+os.path.join(args.output, args.tree)+".")

if __name__ == "__main__":
	main()

