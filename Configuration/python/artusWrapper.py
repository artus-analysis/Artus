
# -*- coding: utf-8 -*-

import argparse
import logging
import sys
import os

import Artus.Configuration.logger as logger
import Artus.Configuration.jsonTools as jsonTools


def artusWrapper(defaultExecutable=None):
	parser = argparse.ArgumentParser(description="Artus executable wrapper script", parents=[logger.loggingParser])

	parser.add_argument("-c", "--config-files", help="JSON Config files.", nargs="+", required=True)
	parser.add_argument("-i", "--input-files", help="Input root files.", nargs="+", required=True)
	parser.add_argument("-o", "--output-file", help="Output root file. [Default: output.root]", default="output.root")
	
	if defaultExecutable: parser.add_argument("-x", "--executable", help="Artus executable. [Default: %s]" % str(defaultExecutable), default=defaultExecutable)
	else: parser.add_argument("-x", "--executable", help="Artus executable.", required=True)

	args = parser.parse_args()
	logger.initLogger(args)
	
	# constuct main JSON config
	mainConfig = {
		"InputFiles" : args.input_files,
		"OutputPath" : args.output_file,
	}
	mainConfig = jsonTools.mergeJsonDicts(mainConfig, *args.config_files)
	mainConfigFileName = reduce(lambda a, b: a+"__"+b, map(lambda fileName: os.path.splitext(os.path.basename(fileName))[0], args.config_files))+".json"
	jsonTools.saveJsonDict(mainConfig, mainConfigFileName)
	logging.getLogger(__name__).info("Saved combined JSON config \"%s\" for temporary usage." % mainConfigFileName)
	
	# call C++ executable
	command = args.executable + " " + mainConfigFileName
	logging.getLogger(__name__).info("Execute \"%s\"." % command)
	os.system(command)
	
	# remove tmp. config
	logging.getLogger(__name__).info("Remove temporary config file.")
	os.system("rm " + mainConfigFileName)

