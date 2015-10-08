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


def main():
	
	ROOT.gSystem.Load(os.path.expandvars("$CMSSW_BASE/src/Kappa/lib/libKappa"))
	
	parser = argparse.ArgumentParser(description="Print out the generator weight.",
	                                 parents=[logger.loggingParser])

	parser.add_argument("files", nargs="+", help="Input files.")
	parser.add_argument("-n", "--nick", default="kappa_(?P<nick>.*)_\d+.root",
	                    help="Regular expression to extract nickname from file names. [Default: %(default)s]")

	args = parser.parse_args()
	logger.initLogger(args)
	
	sumweight_per_nick = {}
	n_entries_per_nick = {}
	for (fileindex, file_name) in enumerate(args.files):

		# retrieve weights
		# the weights are in the form +/- X
		root_file = ROOT.TFile(file_name, "READ")
		eventsTree = root_file.Get("Events")
		n_entries = eventsTree.GetEntries()

		weightHisto = ROOT.TH1F("weightHisto", "histo of weights", 1000, 0.5, 1.5)
		eventsTree.Draw("1>>weightHisto", "eventInfo.weight/fabs(eventInfo.weight)", "goff")

		# nickname matching and sum of weights
		nick = re.match(args.nick, os.path.basename(file_name)).groupdict().values()[0]
		sumweight_per_nick[nick] = sumweight_per_nick.get(nick, 0.0) + weightHisto.GetSumOfWeights()
		n_entries_per_nick[nick] = n_entries_per_nick.get(nick, 0.0) + n_entries

		# counter
		if (fileindex % 200 == 0):
			print "...processing file %d (nickname: %s)" % (fileindex, nick)
			print "\tweighted entries = ", sumweight_per_nick[nick]
			print "\tentries = ", n_entries_per_nick[nick]

		# delete histogram and close rootfile
		weightHisto.Delete()
		root_file.Close()


	# print results
	log.info("{\n\t\"GeneratorWeight\" : {\n\t\t\"nick\" : {")
	for index, (nick, sumweight) in enumerate(sumweight_per_nick.items()):
		log.info("\t\t\t\"" + nick + "\" : " + str(sumweight/n_entries_per_nick[nick]) + ("," if index < len(sumweight_per_nick)-1 else ""))
	log.info("\t\t}\n\t}\n}")


if __name__ == "__main__":
	main()

