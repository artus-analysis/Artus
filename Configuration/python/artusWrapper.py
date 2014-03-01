
# -*- coding: utf-8 -*-

import argparse
import logging
import sys
import os

import Artus.Utility.logger as logger
import Artus.Configuration.jsonTools as jsonTools


def artusWrapper(defaultExecutable=None):
	parser = argparse.ArgumentParser(description="Wrapper for Artus executables. This wrapper can handle simple merges of all given config files. In the resulting JSON config, include statements are frist replaced by the actual config and comments are taken out.",
	                                 parents=[logger.loggingParser])

	configGroup = parser.add_mutually_exclusive_group(required=True)
	configGroup.add_argument("-c", "--config-files", help="JSON config files or string represenations of dicts. All configs are merged, where the first ones in the list have a higher priority than later ones.", nargs="+")
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
	mainConfig = jsonTools.JsonDict(jsonTools.JsonDictList([mainConfig]+[args.config_files]))
	
	# treat includes and comments
	mainConfig = mainConfig.doIncludes().doComments()
	
	mainConfigFileName = reduce(lambda a, b: a+"__"+b, map(lambda fileName: os.path.splitext(os.path.basename(fileName))[0], args.config_files))+".json"
	mainConfig.save(mainConfigFileName)
	logging.getLogger(__name__).info("Saved combined JSON config \"%s\" for temporary usage." % mainConfigFileName)
	
	# call C++ executable
	command = args.executable + " " + mainConfigFileName
	logging.getLogger(__name__).info("Execute \"%s\"." % command)
	exitCode = os.system(command)
	if exitCode != 0:
		logging.getLogger(__name__).error("Exit with code %s.\n\n" % exitCode)
		logging.getLogger(__name__).info("Dump configuration:\n")
		print jsonTools.JsonDict(mainConfigFileName)
	
	# remove tmp. config
	logging.getLogger(__name__).info("Remove temporary config file.")
	os.system("rm " + mainConfigFileName)
	
	if exitCode < 256: return exitCode
	else: return 1 # Artus sometimes returns exit codes >255 that are not supported

