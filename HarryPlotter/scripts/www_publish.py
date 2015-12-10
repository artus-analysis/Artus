#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)
import sys

from Artus.HarryPlotter.plotdata import PlotData
print sys.argv[1]
import argparse
parser = argparse.ArgumentParser(description="Make Data-MC control plots.",
                                 parents=[logger.loggingParser])
parser.add_argument("-i", "--input-dirs", required=True, nargs="+",
                    help="Input directories")
parser.add_argument("--output-subfolders", default=[""], nargs="+",
                    help="Output Subfolders")
args = parser.parse_args()
logger.initLogger(args)

if(args.input_dirs > 0):
	if(len(args.input_dirs) != len(args.output_subfolders)):
		log.fatal("you tried to use " + str(len(args.input_dirs)) + " input directoreis but " + str(len(args.output_subfolders)) + ". This might cause overwriting of files.")
		sys.exit()

for input_dir, subfolder in zip(args.input_dirs, args.output_subfolders):
	PlotData.webplotting(
	              www = subfolder,
	             output_dir = input_dir,
	              export_json = False
	              )
