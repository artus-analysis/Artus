#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import glob
import os
import sys


def create_filelist(files, filename, symlink=None):
	with open(filename, "w") as filelist:
		filelist.write("\n".join(files))
		log.info("Created %s." % filename)
		if not symlink is None:
			logger.subprocessCall(("ln -fsv %s %s" % (os.path.relpath(filename, os.path.dirname(symlink)), symlink)).split())


def main():

	parser = argparse.ArgumentParser(description="Create filelists for Artus inputs from pnfs mount on the NAF.", parents=[logger.loggingParser])

	parser.add_argument("-s", "--skimming-dir", required=True,
	                    help="Skimming output directory as specified in se (output) path in GC.")
	parser.add_argument("-d", "--date", required=True,
	                    help="Date string.")
	parser.add_argument("-o", "--output-dir", default="$CMSSW_BASE/src/HiggsAnalysis/KITHiggsToTauTau/data/Samples/",
	                    help="Output directory for the filelists. [Default: %(default)s]")
	parser.add_argument("-r", "--create-recent-symlinks", default=False, action="store_true",
	                    help="Create symlinks to filelists as recent ones. [Default: %(default)s]")

	args = parser.parse_args()
	logger.initLogger(args)
	
	if not args.skimming_dir.startswith("/pnfs"):
		log.critical("You need to specify a /pnfs path at the NAF")
		sys.exit(1)
	
	args.output_dir = os.path.expandvars(args.output_dir)
	
	skimming_dirs = glob.glob(os.path.join(args.skimming_dir, "*_*TeV*"))
	
	for skimming_dir in skimming_dirs:
		nick = os.path.basename(skimming_dir)
		files = sorted(glob.glob(os.path.join(skimming_dir, "*.root")))
		filelists = os.path.join(args.output_dir, "%s_sample_%s_%s.txt" % ("%s", nick, "%s"))
		
		dcache_settings = {
			"NAF" : ["", None],
			"DCAP" : ["dcap://dcache-cms-dcap.desy.de/", None],
			"XROOTD" : ["root://cms-xrd-global.cern.ch/", "/pnfs/desy.de/cms/tier2"],
		}
		for name, settings in dcache_settings.items():
			create_filelist([(settings[0] + (root_file.replace(settings[1], "") if settings[1] else root_file)) for root_file in files],
			                filelists % (name, args.date),
			                filelists % (name, "recent") if args.create_recent_symlinks else None)


if __name__ == "__main__":
	main()

