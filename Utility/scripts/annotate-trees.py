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

import Artus.Utility.progressiterator as progressiterator
import Artus.Utility.tfilecontextmanager as tfilecontextmanager
import Artus.HarryPlotter.utility.roottools as roottools


def main():
	
	parser = argparse.ArgumentParser(description="Add friend to tree in file with constant values.",
	                                 parents=[logger.loggingParser])
	
	parser.add_argument("files", nargs="+",
	                    help="Files containing the tree. The files will be updated.")
	parser.add_argument("-t", "--tree", required=True,
	                    help="Path to the tree object in the file.")
	parser.add_argument("--values", nargs="+", required=True,
	                    help="Values to add to the friend tree. Each value gets a separate branch.")
	parser.add_argument("-b", "--branches", nargs="+", default=[None],
	                    help="Branch names.")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	if len(args.branches) < len(args.values):
		args.branches = (args.branches+(len(args.values)*[None]))[:len(args.values)]
	args.branches = ["annotation%d" % i if b is None else b for i, b in enumerate(args.branches)]
	
	types = {
		bool : "O",
		int : "I",
		float : "D",
		str : "C",
	}
	value_types = []
	for index, value in enumerate(args.values):
		try:
			value = int(value)
			value_types.append(int)
		except:
			try:
				value = float(value)
				value_types.append(float)
			except:
				try:
					# TODO: check if type is bool
					#value = bool(value)
					#value_types.append(bool)
					raise Exception()
				except:
					# TODO: find way to branch strings
					#value_types.append(str)
					raise Exception()
		args.values[index] = value
	
	log.info("New branches:")
	for index, (branch, value_type, value) in enumerate(zip(args.branches, value_types, args.values)):
		log.info("\t%s/%s = %s" % (branch, types[value_type], str(value)))
	
	for file_name in progressiterator.ProgressIterator(args.files, description="Processing files"):
		with tfilecontextmanager.TFileContextManager(file_name, "UPDATE") as root_file:
			tree = root_file.Get(args.tree)
			
			dir_name = os.path.dirname(args.tree)
			if not dir_name == "":
				root_file.Get(dir_name)
			
			elements = zip(*roottools.RootTools.walk_root_directory(root_file))[-1]
			friend_tree_name = None
			n_trials = 0
			while friend_tree_name is None:
				tmp_friend_tree_name = (tree.GetName()+"_friend_"+str(n_trials)).rstrip("_0")
				if not tmp_friend_tree_name in elements:
					friend_tree_name = tmp_friend_tree_name
				n_trials += 1
			friend_tree = ROOT.TTree(friend_tree_name, tree.GetTitle()+" (friend)")
			
			values = []
			for branch, value_type, value in zip(args.branches, value_types, args.values):
				values.append(numpy.zeros(1, dtype=value_type))
				values[-1][0] = value
				friend_tree.Branch(branch, values[-1], "%s/%s" % (branch, types[value_type]))
			
			for entry in xrange(tree.GetEntries()):
				friend_tree.Fill()
			
			friend_tree.AddFriend(tree, tree.GetName())
			tree.AddFriend(friend_tree, friend_tree.GetName())
			
			root_file.Write()


if __name__ == "__main__":
	main()

