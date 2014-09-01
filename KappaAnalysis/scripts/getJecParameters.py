#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import os
import string
import tempfile


def main():

	parser = argparse.ArgumentParser(description="Get JEC parameter files using CMSSW tools.", parents=[logger.loggingParser])
	parser.add_argument("--gt-data", default="FT_53_V21_AN4::All",
	                    help="Global tag for data. [Default: %(default)s]")
	parser.add_argument("--gt-mc", default="START53_V23::All",
	                    help="Global tag for MC. [Default: %(default)s]")
	parser.add_argument("--pf-jets", default="AK5PF",
	                    help="Name of PF jets collection. [Default: %(default)s]")
	parser.add_argument("--calo-jets", default="",
	                    help="Name of Calo jets collection. [Default: %(default)s]")
	args = parser.parse_args()
	logger.initLogger(args)
	args = vars(args)
	
	python_config_template = string.Template("""
import FWCore.ParameterSet.Config as cms
process = cms.Process("jectxt")
process.load("Configuration.StandardSequences.Services_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
# define your favorite global tag
process.GlobalTag.globaltag = "$global_tag"
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1))
process.source = cms.Source("EmptySource")
process.readAK5PF    = cms.EDAnalyzer("JetCorrectorDBReader",
      payloadName    = cms.untracked.string("$pf_jets"),
      # this is used ONLY for the name of the printed txt files. You can use any name that you like,
      # but it is recommended to use the GT name that you retrieved the files from.
      globalTag      = cms.untracked.string("$global_tag_name"),
      printScreen    = cms.untracked.bool(False),
      createTextFile = cms.untracked.bool(True)
)
process.readAK5Calo = process.readAK5PF.clone(payloadName = "$calo_jets")
process.p = cms.Path(process.readAK5PF * process.readAK5Calo)
""")
	
	for mode in ["data", "mc"]:
		python_config = python_config_template.substitute(global_tag=args["gt_"+mode],
		                                                  global_tag_name=args["gt_"+mode].replace(":", "_"),
		                                                  pf_jets=args["pf_jets"],
		                                                  calo_jets=args["calo_jets"])
		if args["calo_jets"] == "":
			python_config = python_config.replace("\nprocess.readAK5Calo = process.readAK5PF.clone(payloadName = \"\")", "")
			python_config = python_config.replace("process.p = cms.Path(process.readAK5PF * process.readAK5Calo)",
			                                      "process.p = cms.Path(process.readAK5PF)")
		
		
		python_config_file_name = None
		with tempfile.NamedTemporaryFile(prefix=mode, suffix=".py", delete=False) as python_config_file:
			python_config_file_name = python_config_file.name
			python_config_file.write(python_config)
		
		if python_config_file_name != None:
			command = ["cmsRun", python_config_file_name]
			log.info("Execute \"%s\"." % " ".join(command))
			exit_code = logger.subprocessCall(command)
			log.info("Exit code: " + str(exit_code))
			os.remove(python_config_file_name)


if __name__ == "__main__":
	main()

