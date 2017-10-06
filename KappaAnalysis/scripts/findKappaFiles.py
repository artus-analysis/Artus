#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os

import Artus.Utility.jsonTools as jsonTools

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True


def main():
	ROOT.gROOT.SetBatch(True)
	
	parser = argparse.ArgumentParser(description="Search for Kappa files containing certain events.",
	                                 parents=[logger.loggingParser])
	
	parser.add_argument("files", nargs="+", help="Kappa skim output files to check")
	parser.add_argument("-c", "--config", help="JSON config file containing the event selection.")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	config = jsonTools.JsonDict(args.config)
	
	run_whitelist = config.get("RunWhitelist", [])
	lumi_whitelist = config.get("LumiWhitelist", [])
	event_whitelist = config.get("EventWhitelist", [])
	
	run_blacklist = config.get("RunBlacklist", [])
	lumi_blacklist = config.get("LumiBlacklist", [])
	event_blacklist = config.get("EventBlacklist", [])
	
	selections = []
	
	if len(run_whitelist) > 0:
		selections.append("||".join(["(eventInfo.nRun==%d)" % run for run in run_whitelist]))
	if len(lumi_whitelist) > 0:
		selections.append("||".join(["(eventInfo.nLumi==%d)" % lumi for lumi in lumi_whitelist]))
	if len(event_whitelist) > 0:
		selections.append("||".join(["(eventInfo.nEvent==%d)" % event for event in event_whitelist]))
	
	if len(run_blacklist) > 0:
		selections.append("&&".join(["(eventInfo.nRun!=%d)" % run for run in run_blacklist]))
	if len(lumi_blacklist) > 0:
		selections.append("&&".join(["(eventInfo.nLumi!=%d)" % lumi for lumi in lumi_blacklist]))
	if len(event_blacklist) > 0:
		selections.append("&&".join(["(eventInfo.nEvent!=%d)" % event for event in event_blacklist]))
	
	selection = "&&".join(["(%s)" % cut for cut in selections])
	
	results = []
	for input_file in args.files:
		tree = ROOT.TChain("Events")
		tree.AddFile(input_file)
		entries = tree.Draw("1", selection, "goff")
		if entries > 0:
			results.append((entries, input_file))
		log.info("%4d entries: %s" % (entries, input_file))

	log.info("\nFound files:")
	for entries, input_file in results:
		log.info("%4d entries: %s" % (entries, input_file))

if __name__ == "__main__":
	main()

