#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os
import re
from datetime import datetime

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError

from Kappa.Skimming.registerDatasetHelper import *

def main():
	
	ROOT.gSystem.Load(os.path.expandvars("$ARTUSPATH/../Kappa/lib/libKappa"))
	
	parser = argparse.ArgumentParser(description="Print out the generator weight.",
	                                 parents=[logger.loggingParser])

	parser.add_argument("files", nargs="+", help="Input files.")
	parser.add_argument("-n", "--nick", default="kappa_(?P<nick>.*)_\d+.root",
	                    help="Regular expression to extract nickname from file names. Use 'NONE' to disable nick name matching. [Default: %(default)s]")

	args = parser.parse_args()
	logger.initLogger(args)
	
	sumweight_per_nick = {}
	n_entries_per_nick = {}
	for (fileindex, file_name) in enumerate(args.files):
		
		# retrieve weights and total number of events
		root_file = ROOT.TFile(file_name, "READ")
		lumiTree = root_file.Get("Lumis")
		n_entries = lumiTree.GetEntries()
		
		# nickname matching and sum of weights
		no_regex_match = True if args.nick=='NONE' else False
		nick = ( re.match(args.nick, os.path.basename(file_name)).groupdict().values()[0] if not no_regex_match else '*')
		
		for entry in xrange(n_entries):
			lumiTree.GetEntry(entry)
			n_entries_per_nick[nick] = n_entries_per_nick.get(nick, 0.0) + lumiTree.filterMetadata.nEventsTotal
			sumweight_per_nick[nick] = sumweight_per_nick.get(nick, 0.0) + (lumiTree.filterMetadata.nEventsTotal - 2 * lumiTree.filterMetadata.nNegEventsTotal)

		#counter
		if (fileindex % 100 == 0):
			print "...processing file %d (nickname: %s)" % (fileindex, nick)
			print "\tweighted entries = ", sumweight_per_nick[nick]
			print "\tentries = ", n_entries_per_nick[nick]

		#close rootfile
		root_file.Close()

	# print results and save to dataset
	if not no_regex_match: 
		dataset = os.path.expandvars("$ARTUSPATH/../Kappa/Skimming/data/datasets.json")
		dictionary = load_database(dataset)
	for index, (nick, sumweight) in enumerate(sumweight_per_nick.items()):
		if not no_regex_match: 
			sample_name = get_sample_by_nick(nick)
			dictionary[sample_name]["n_events_generated"] = str(int(n_entries_per_nick[nick]))
			dictionary[sample_name]["generatorWeight"] = sumweight/n_entries_per_nick[nick]

			log.info("\n\n\"" + sample_name + "\"" + ": {")
		log.info("\tn_events_generated: " + str(int(n_entries_per_nick[nick])))
		log.info("\tgeneratorWeight: " + str(sumweight/n_entries_per_nick[nick]))
	
	log.info("}")
	if not no_regex_match: save_database(dictionary, dataset)



if __name__ == "__main__":
	main()

