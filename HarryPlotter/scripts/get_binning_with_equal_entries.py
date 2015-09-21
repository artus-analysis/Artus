#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse

import Artus.HarryPlotter.analysis_modules.eventselectionoverlap as eventselectionoverlap
from Artus.HarryPlotter.utility.tfilecontextmanager import TFileContextManager


def get_binning_with_equal_entries():
	"""Get binning with roughly equal number of entries per bin
	
		Example usage: get_binning_with_equal_entries.py artus_output.root  folder/ntuple  quantityname  --n-bins 10
	"""

	parser = argparse.ArgumentParser(description="Print binning of histograms", parents=[logger.loggingParser])

	parser.add_argument("root_file", help="Input ROOT file")
	parser.add_argument("folder", help="Folder to TTree")
	parser.add_argument("quantity", help="Quantity for which binning should be determined")
	parser.add_argument("--n-bins", type=int, help="Number of bins. [Default: %(default)s]", default=25)
	parser.add_argument("--selection", type=str, help="Optional selection", default="1")

	args = parser.parse_args()
	logger.initLogger(args)
	
	with TFileContextManager(args.root_file, "READ") as root_file:
		tree = root_file.Get(args.folder)

		events_list = sorted([e[0] for e in eventselectionoverlap.EventSelectionOverlap.get_events_list_from_tree(tree, args.quantity, args.selection)])
		assert(len(events_list) > args.n_bins)

		# slice events list
		bin_border_list = []
		for i_bin in range(args.n_bins):
			bin_border_list.append(events_list[int(round(i_bin/float(args.n_bins)*len(events_list)))])
		bin_border_list.append(events_list[-1])
		
		log.info("Bin borders for {} bins:".format(args.n_bins))
		log.info(" ".join([str(i) for i in bin_border_list]))


if __name__ == "__main__":
	get_binning_with_equal_entries()
