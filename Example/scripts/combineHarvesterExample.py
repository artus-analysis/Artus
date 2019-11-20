#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os

import CombineHarvester.CombineTools.ch as ch


def write_datacards(cb, datacard_filename_templates, root_filename_template, output_directory):
	for datacard_filename_template in datacard_filename_templates:
		writer = ch.CardWriter(os.path.join("$TAG", datacard_filename_template), os.path.join("$TAG", root_filename_template))
	
		if (len(cb.mass_set()) == 1) and (cb.mass_set()[0] == "*"):
			writer.SetWildcardMasses([])
		
		writer.WriteCards(output_directory, cb)


if __name__ == "__main__":
	
	parser = argparse.ArgumentParser(description="Simple example for Combine(Harvester)", parents=[logger.loggingParser])
	
	parser.add_argument("-o", "--output-directory", default=".",
	                    help="Output directory. [Default: %(default)s]")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	analysis = "analysis"
	era = "era"
	channel = "channel"
	categories = {
		"inclusive" : 0,
		"cat_sig" : 1,
		"cat_bkg" : 2,
	}
	processes = [
		"sig",
		"bkg",
	]
	mass = "*"
	
	cb = ch.CombineHarvester()
	
	for category, bin_id in categories.iteritems():
		cb.AddObservations(analysis=[analysis], era=[era], channel=[channel], bin=[(bin_id, category)], mass=[mass])
		cb.AddProcesses(analysis=[analysis], era=[era], channel=[channel], bin=[(bin_id, category)], mass=[mass], procs=["bkg"], signal=False)
		cb.AddProcesses(analysis=[analysis], era=[era], channel=[channel], bin=[(bin_id, category)], mass=[mass], procs=["sig"], signal=True)
	
	for category in categories.keys():
		for process in processes:
			cb.cp().bin([category]).process([process]).AddSyst(cb, "uncorrelated_"+category+"_proc_"+process, "lnN", ch.SystMap()(1.2))
		
		cb.cp().bin([category]).AddSyst(cb, "correlated_"+category, "lnN", ch.SystMap()(1.2))
	
	for process in processes:
		cb.cp().process([process]).AddSyst(cb, "correlated_proc_"+process, "lnN", ch.SystMap()(1.2))
	
	cb.cp().AddSyst(cb, "correlated", "lnN", ch.SystMap()(1.2))
	
	input_file = os.path.expandvars("$CMSSW_BASE/src/Artus/Example/data/combine/sig_bkg_2categories_noshapes.root")
	cb.ExtractShapes(input_file, "$BIN/$PROCESS", "$BIN/$PROCESS_$SYSTEMATIC")
	
	if log.isEnabledFor(logging.DEBUG):
		cb.PrintAll()
	
	cb_categories = cb.cp().bin(["cat_sig", "cat_bkg"])
	root_filename_template = "datacards/common/templates.root"
	write_datacards(cb_categories, ["datacards/categories_combined/datacard.txt"], root_filename_template, args.output_directory)
	write_datacards(cb, ["datacards/per_category/$BIN/datacard.txt"], root_filename_template, args.output_directory)

