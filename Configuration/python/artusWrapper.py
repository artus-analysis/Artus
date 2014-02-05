
# -*- coding: utf-8 -*-

import argparse
import logging
import sys
import os

import Artus.Configuration.logger as logger
import Artus.Configuration.jsonTools as jsonTools


def artusWrapper(defaultExecutable=None):
	parser = argparse.ArgumentParser(description="Artus executable wrapper script", parents=[logger.loggingParser])

	configGroup = parser.add_mutually_exclusive_group(required=True)
	configGroup.add_argument("-c", "--config-files", help="JSON config files or string represenations of dicts", nargs="+")
	configGroup.add_argument("-r", "--root-config", help="Take JSON configuration from existing root file.", nargs=1)
	
	parser.add_argument("-i", "--input-files", help="Input root files. Leave empty (\"\") if input files from root file should be taken.", nargs="+", required=True)
	parser.add_argument("-o", "--output-file", help="Output root file. [Default: output.root]", default="output.root")
	
	if defaultExecutable: parser.add_argument("-x", "--executable", help="Artus executable. [Default: %s]" % str(defaultExecutable), default=defaultExecutable)
	else: parser.add_argument("-x", "--executable", help="Artus executable.", required=True)

	args = parser.parse_args()
	logger.initLogger(args)
	
	if args.config_files and args.input_files == "":
		logging.getLogger(__name__).critial("No input file specified!")
		sys.exit(1)
	
	# constuct main JSON config
	mainConfig = {
		"OutputPath" : args.output_file,
	}
	if args.config_files: mainConfig["InputFiles"] = args.input_files
	else: args.config_files = args.root_config
	mainConfig = jsonTools.mergeJsonDicts(mainConfig, *args.config_files)
	
	mainConfigFileName = reduce(lambda a, b: a+"__"+b, map(lambda fileName: os.path.splitext(os.path.basename(fileName))[0], args.config_files))+".json"
	jsonTools.saveJsonDict(mainConfig, mainConfigFileName)
	logging.getLogger(__name__).info("Saved combined JSON config \"%s\" for temporary usage." % mainConfigFileName)
	
	# call C++ executable
	command = args.executable + " " + mainConfigFileName
	logging.getLogger(__name__).info("Execute \"%s\"." % command)
	exitCode = os.system(command)
	if exitCode != 0:
		logging.getLogger(__name__).error("Exit with code %s.\n\n" % exitCode)
		logging.getLogger(__name__).info("Dump configuration:\n")
		jsonTools.printJsonDict(mainConfigFileName)
		sys.exit(exitCode)
	
	# remove tmp. config
	logging.getLogger(__name__).info("Remove temporary config file.")
	os.system("rm " + mainConfigFileName)

