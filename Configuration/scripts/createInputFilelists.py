#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import glob
import os
import sys
from Kappa.Skimming.registerDatasetHelper import get_n_files_from_nick
from math import ceil
job_submission_limit = 10000 # crab3 can only submit up to 10000 jobs per task

def create_symlink(symlink, filename):
	logger.subprocessCall(("ln -fsv %s %s" % (os.path.relpath(filename, os.path.dirname(symlink)), symlink)).split())

def create_filelist(files, filename, symlink=None):
	with open(filename, "w") as filelist:
		filelist.write("\n".join(files))
		log.info("Created %s." % filename)
		if not symlink is None:
			create_symlink(symlink,filename)

def main():

	parser = argparse.ArgumentParser(description="Create filelists for Artus inputs from pnfs mount on the NAF or import already existing filelists.", parents=[logger.loggingParser])

	parser.add_argument("-s", "--skimming-dir", required=True,
	                    help="Skimming output directory as specified in se (output) path in GC.")
	parser.add_argument("--if, --import-filelists-folder",dest="import_filelists_folder", default=None,
	                    help="The folder, where the filelists which should be imported can be found. [Default: %(default)s]")
	parser.add_argument("-d", "--date", required=True,
	                    help="Date string.")
	parser.add_argument("-o", "--output-dir", default="$CMSSW_BASE/src/HiggsAnalysis/KITHiggsToTauTau/data/Samples/",
	                    help="Output directory for the filelists. [Default: %(default)s]")
	parser.add_argument("-r", "--create-recent-symlinks", default=False, action="store_true",
	                    help="Create symlinks to filelists as recent ones. [Default: %(default)s]")
	parser.add_argument("-c", "--crab", dest="crab", default=True, action="store_true",
	                    help="Skimming output directory was created by Crab (it has a different structure). [Default: %(default)s]")
	parser.add_argument("--gc", dest="crab", default=True, action="store_false",
	                    help="Skimming output directory was created by Grid-Control (it has a different structure). [Default: not %(default)s]")
	parser.add_argument("--no-strict-checking", default=False, action="store_true",
	                    help="Disable check, if number of files in DBS is the same as Kappa files to be added to the file list. [Default: not %(default)s]")
	parser.add_argument("--units-per-job", default=1, type=int,
	                    help="Set the number of files per job you set in your crab campaign [Default: %(default)s]")

	args = parser.parse_args()
	logger.initLogger(args)

	if args.import_filelists_folder:
		print "Importing and linking existing filelists in", args.import_filelists_folder
		for (dirpath, dirnames, filenames) in os.walk(args.import_filelists_folder):
			for f in filenames:
				path = os.path.join(dirpath,f)
				# it is assumed, that the filenames are created by the SkimManager and represent dataset nicknames
				if os.path.exists(path) and ".txt" in path:
					create_symlink("XROOTD_sample_"+os.path.basename(path).replace(".txt","_recent.txt"), path)
		sys.exit(0)

	if not args.skimming_dir.startswith("/pnfs"):
		log.critical("You need to specify a /pnfs path at the NAF")
		sys.exit(1)
	
	args.output_dir = os.path.expandvars(args.output_dir)
	
	skimming_dirs = glob.glob(os.path.join(args.skimming_dir, "*_*TeV*"))
	if args.crab:
		skimming_dirs = glob.glob(os.path.join(args.skimming_dir, "*/crab_*"))
	
	for skimming_dir in skimming_dirs:
		search_path = os.path.join(skimming_dir, "*/*.root")
		if args.crab:
			search_path = os.path.join(skimming_dir, "*/*/*.root")
		
		files = sorted(glob.glob(search_path))

		if (len(files) == 0):
			log.warning("Cannot find any files matching \"%s\"." % search_path)
			continue
		nick = os.path.basename(files[0]).replace("kappa_", "")
		nick = nick[:nick.rfind("_")]
		n_files_from_nick = get_n_files_from_nick(nick)
		print nick
		print n_files_from_nick," files"
		if args.units_per_job > 1:
			n_files_from_nick = ceil(float(n_files_from_nick) / float(args.units_per_job))
			print int(n_files_from_nick)," files (units-per-job > 1)"
		if float(n_files_from_nick) >= job_submission_limit:
			files_per_job = ceil(float(n_files_from_nick) / job_submission_limit)
			n_files_from_nick = ceil(float(n_files_from_nick) / files_per_job)
			print int(n_files_from_nick)," files (hit the job_submission_limit)"
		if ((len(files) != int(n_files_from_nick)) and (not args.no_strict_checking)):
			log.error("Found %s files, but expected %s from nick %s. No filelist for this sample is saved. Disable this check with --no-strict-checking" % (len(files), get_n_files_from_nick(nick), nick))
		else:
			filelists = os.path.join(args.output_dir, "%s_sample_%s_%s.txt" % ("%s", nick, "%s"))
			dcache_settings = {
				#"NAF" : ["", None],
				#"DCAP" : ["dcap://dcache-cms-dcap.desy.de/", None],
				"XROOTD" : ["root://cms-xrd-global.cern.ch/", "/pnfs/desy.de/cms/tier2"],
			}
			for name, settings in dcache_settings.items():
				create_filelist(
						[(settings[0] + (root_file.replace(settings[1], "") if settings[1] else root_file)) for root_file in files],
						filelists % (name, args.date),
						filelists % (name, "recent") if args.create_recent_symlinks else None
				)


if __name__ == "__main__":
	main()

