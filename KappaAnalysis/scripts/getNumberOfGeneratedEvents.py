#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os
import re

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError


def main():
	
	ROOT.gSystem.Load(os.path.expandvars("$CMSSW_BASE/src/Kappa/lib/libKappa"))
	
	parser = argparse.ArgumentParser(description="Print out the sum of entries in the Event trees of all inputs sorted by nick names.",
	                                 parents=[logger.loggingParser])

	parser.add_argument("files", nargs="+", help="Input files.")
	parser.add_argument("-n", "--nick", default="kappa_(?P<nick>.*)_\d+.root",
	                    help="Regular expression to extract nickname from file names. [Default: %(default)s]")

	args = parser.parse_args()
	logger.initLogger(args)
	
	n_entries_per_nick = {}
	for file_name in args.files:
	
		# retrieve number of entries
		root_file = ROOT.TFile(file_name, "READ")
		events = root_file.Get("Events")
		n_entries = events.GetEntries()
		root_file.Close()
		
		# nickname matching and sum of entries
		nick = re.match(args.nick, os.path.basename(file_name)).groupdict().values()[0]
		n_entries_per_nick[nick] = n_entries_per_nick.get(nick, 0.0) + n_entries
	
	# print results
	log.info("{\n\t\"NumberGeneratedEvents\" : {\n\t\t\"nick\" : {")
	for index, (nick, n_entries) in enumerate(n_entries_per_nick.items()):
		log.info("\t\t\t\"" + nick + "\" : " + str(int(n_entries)) + ("," if index < len(n_entries_per_nick)-1 else ""))
	log.info("\t\t}\n\t}\n}")


if __name__ == "__main__":
	main()

