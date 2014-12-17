#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os
import shlex
import sys

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError

import Artus.Utility.tools as tools


def main():
	ROOT.gROOT.SetBatch(True)
	ROOT.gSystem.Load(os.path.expandvars("$CMSSW_BASE/src/Kappa/lib/libKappa"))
	
	parser = argparse.ArgumentParser(description="Switch to the Kappa commit that has been used for a certain skim.",
	                                 parents=[logger.loggingParser])
	
	parser.add_argument("file", help="Kappa skim output file containing the UserInfo in the Lumis tree")
	parser.add_argument("-a", "--args", default="",
	                    help="Arguments for git checkout. [Default: %(default)s]")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	root_file = ROOT.TFile(args.file, "READ")
	lumis = root_file.Get("Lumis")
	lumis.GetEntry(0)
	
	user_infos = lumis.GetUserInfo()
	assert user_infos.GetEntries() >= 2
	keys = user_infos[0]
	values = user_infos[1]
	
	user_infos_dict = { str(k.GetString()) : str(v.GetString()) for k, v in zip(keys, values) }
	
	cwd = os.getcwd()
	if not cwd.endswith("Kappa") or cwd.endswith("Kappa/"):
		log.fatal("The script needs to be executed in $CMSSW_BASE/src/Kappa!")
		sys.exit(1)
	
	kappa_repo = [k for k in user_infos_dict.keys() if k.endswith("Kappa") or k.endswith("Kappa/")]
	if len(kappa_repo) == 0:
		log.fatal("Kappa revision not found in file \"%s\"!" % args.file)
		sys.exit(1)
	else:
		kappa_repo = kappa_repo[0]
		
	#sorted([tools.longest_common_substring(key, cwd) for key in user_infos_dict.keys()], key=lambda item: len(item))[-1]
	kappa_revision = user_infos_dict[kappa_repo]
	
	command = "git checkout %s %s" % (args.args, kappa_revision)
	log.info(command)
	if not raw_input("Proceed? [Y|n] ").lower().strip().startswith("n"):
		logger.subprocessCall(shlex.split(command))


if __name__ == "__main__":
	main()

