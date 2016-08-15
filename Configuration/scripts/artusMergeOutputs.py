#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import glob
import os
import sys

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError

import Artus.Utility.tools as tools
import Artus.Utility.progressiterator as pi
from Artus.Utility.tools import hadd2

def folders_to_merge(args):
	outputs_per_nick = {}
	for project_dir in args.project_dir:
		extra_path = "output/" if (os.path.isdir(os.path.join(project_dir, "output"))) else ""
		output_dirs = glob.glob(os.path.join(project_dir, extra_path+"*"))
		nick_names = [nick for nick in [output_dir[output_dir.rfind("/")+1:] for output_dir in output_dirs] if not ".tar.gz" in nick]
		for nick in nick_names:
			if not (nick in outputs_per_nick):
				outputs_per_nick[nick] = []
			outputs_per_nick[nick] = outputs_per_nick[nick] + glob.glob(os.path.join(project_dir, extra_path, nick, "*.root"))
	return outputs_per_nick

def merge_local(args):
	outputs_per_nick = folders_to_merge(args)
	if(args.project_subdir != None): # keep only single path
		outputs_per_nick = { args.project_subdir : outputs_per_nick[args.project_subdir] }
	# drop potentially existing SvfitCaches from the filelist
	for nick, files in outputs_per_nick.iteritems():
		outputs_per_nick[nick] = [file for file in files if ("SvfitCache" not in file)]
	outputs_per_nick = {nick : files for nick, files in outputs_per_nick.iteritems() if len(files) > 0}
	
	hadd_arguments = []
	for nick_name, output_files in pi.ProgressIterator(outputs_per_nick.iteritems(),
	                                                   length=len(outputs_per_nick),
	                                                   description="Merging Artus outputs"):
		merged_dir = os.path.join(args.project_dir[0] if(args.output_dir == None) else args.output_dir, "merged", nick_name)
		if not os.path.exists(merged_dir):
			os.makedirs(merged_dir)
		hadd_arguments.append({"target_file": os.path.join(merged_dir, nick_name+".root"), "source_files": output_files, "hadd_args" : " -f ", "max_files" : 500})
	
	tools.parallelize(hadd2, hadd_arguments, n_processes=args.n_processes)

def merge_batch(args):

	from gcSettings import Settings
	cfg = Settings()
	cfg.workflow.task = 'UserTask'
	cfg.workflow.backend = 'local'

	cfg.jobs.wall_time = '12:00:00'
	cfg.jobs.set("memory", "6000")

	cfg.usertask.executable = 'Artus/Utility/scripts/userjob_epilog.sh'
	cmssw_base = os.getenv("CMSSW_BASE") + "/src/"
	executable = 'artusMergeOutputs.py '
	cfg.usertask.set("input files", [cmssw_base + "Artus/Configuration/scripts/artusMergeOutputs.py"] )

	project_dirs = "-i " + " ".join(args.project_dir)
	print project_dirs
	outputs_per_nick = folders_to_merge(args)
	nicks = outputs_per_nick.keys()
	print nicks
	cfg.parameters.set("parameters", ["NICK"])
	cfg.parameters.set("NICK", nicks)


	arguments = " " .join([executable])
	arguments = arguments + " " .join(args.project_dir)
	arguments = arguments + " --project-subdir @NICK@ "

	cfg.usertask.set('arguments', "%s"%arguments)
	merged_directory = os.path.join(args.project_dir[0], "merged")
	cfg.storage.set('se path', merged_directory )
	cfg.storage.set('se output files', "")
	cfg.storage.set('se output pattern', "@NICK@/@NICK@")
	getattr(cfg, 'global').set('workdir', os.path.join(args.project_dir[0], "workdir_merge"))

	# writing config to file, there's probably a better way
	import StringIO
	stdout = sys.stdout
	gc_config_name = os.path.join(args.project_dir[0], 'artus_merge_outputsWithGC_config.cfg')
	sys.stdout = open( gc_config_name, 'w')
	print(cfg)
	sys.stdout = stdout
	command = "go.py -Gc "  +  gc_config_name

	print "Executing ' " + command + "'"
	logger.subprocessCall(command.split())

	log.info("Output is written to directory \"%s\"" % merged_directory)

	#print stdout
	#print('=' * 20)

def main():
	
	parser = argparse.ArgumentParser(description="Merge Artus outputs per nick name.", parents=[logger.loggingParser])

	parser.add_argument("project_dir", help="Artus Project directory containing the files \"output/*/*.root\" to merge in case there is an output dir, */*.root else", nargs="*")
	parser.add_argument("--project-subdir", help="Artus Project sub-directory in case the script only handles a part", default=None)
	parser.add_argument("-n", "--n-processes", type=int, default=1,
	                    help="Number of (parallel) processes. [Default: %(default)s]")
	parser.add_argument("--output-dir", help="Directory to store merged files. Default: Same as first project_dir.")
	parser.add_argument("-b", "--batch", default=False, const="naf", nargs="?",
	                     help="Run with grid-control. Optionally select backend. [Default: %(default)s]")

	args = parser.parse_args()
	logger.initLogger(args)

	if(args.batch == False):
		merge_local(args)
	else:
		merge_batch(args)

if __name__ == "__main__":
	main()

