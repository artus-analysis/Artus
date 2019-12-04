#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import sys

from Artus.HarryPlotter.plotdata import PlotData


parser = argparse.ArgumentParser(description="Make Data-MC control plots.",
                                 parents=[logger.loggingParser])

parser.add_argument("-i", "--input-dirs", required=True, nargs="+",
                    help="Input directories")
parser.add_argument("-o", "--output-subfolders", default=[""], nargs="+",
                    help="Output Subfolders")
parser.add_argument("--no-publish", default=False, action="store_true", help="only produce overview.html but do not attempt to publish plots")
parser.add_argument("--export-json", default=False, action="store_true", help="export-json")
parser.add_argument("--www-nodate", default=False, action="store_true", help="www_nodate")

args = parser.parse_args()
logger.initLogger(args)

if ((len(args.input_dirs) != len(args.output_subfolders))) and not args.no_publish:
    log.fatal("you tried to use " + str(len(args.input_dirs)) + " input directories but " + str(len(args.output_subfolders)) + " output directories. This might cause overwriting of files. Quit!")
    sys.exit(1)

if args.no_publish:
    args.output_subfolders = args.input_dirs

for input_dir, subfolder in zip(args.input_dirs, args.output_subfolders):
    print subfolder
    PlotData.webplotting(
            www=subfolder,
            output_dir=input_dir,
            export_json=args.export_json,
            www_nodate=args.www_nodate,
            no_publish=args.no_publish
    )
