
# -*- coding: utf-8 -*-

import argparse
import glob
import logging
import os
import sys
import tempfile

import Artus.Utility.logger as logger
import Artus.Configuration.jsonTools as jsonTools


# main function, to be called in user script for each analysis
def artusWrapper(defaultExecutable=None):
	
	args = parseArguments(defaultExecutable)
	
	jsonConfig = constructJsonConfig(args)
	if args.print_config:
		print jsonConfig
	
	# run Artus
	exitCode = 0
	if not args.no_run:
		exitCode = runArtus(args, jsonConfig)
	
	if exitCode < 256: return exitCode
	else: return 1 # Artus sometimes returns exit codes >255 that are not supported


# parse arguments and return the options
def parseArguments(defaultExecutable):

	parser = argparse.ArgumentParser(parents=[logger.loggingParser], fromfile_prefix_chars="@",
	                                 description="Wrapper for Artus executables. JSON configs can be file names pointing to JSON text files or Artus ROOT output files with saved configs or python statements that can be evaluated as dictionary. When JSON configs are merged, the first ones in the list have a higher priority than later ones. In the final config, all includes and comments are replaced accordingly.")
	
	fileOptionsGroup = parser.add_argument_group("File options")
	fileOptionsGroup.add_argument("-i", "--input-files", nargs="+", required=True,
	                              help="Input root files. Leave empty (\"\") if input files from root file should be taken.")
	fileOptionsGroup.add_argument("-o", "--output-file", default="output.root",
	                              help="Output root file. [Default: output.root]")
	fileOptionsGroup.add_argument("-w", "--work", default="$ARTUS_WORK_BASE",
	                              help="Work directory base. [Default: $ARTUS_WORK_BASE]")
	fileOptionsGroup.add_argument("-n", "--project-name",
	                              help="Name for this Artus project specifies the name of the work subdirectory.")

	configOptionsGroup = parser.add_argument_group("Config options")
	configOptionsGroup.add_argument("-c", "--base-configs", nargs="+", required=True,
	                                help="JSON base configurations. All configs are merged.")
	configOptionsGroup.add_argument("-p", "--pipeline-configs", nargs="+", action="append",
	                                help="JSON pipeline configurations. Single entries (whitespace separated strings) are first merged. Then all entries are expanded to get all possible combinations. For each expansion, this option has to be used. Afterwards, all results are merged into the JSON base config.")
	configOptionsGroup.add_argument("-P", "--print-config", default=False, action="store_true",
	                                help="Print out the JSON config before running Artus.")

	runningOptionsGroup = parser.add_argument_group("Running options")
	runningOptionsGroup.add_argument("--no-run", default=False, action="store_true",
	                                 help="Exit before running Artus to only check the configs.")
	runningOptionsGroup.add_argument("-r", "--root", default=False, action="store_true",
	                                 help="Open output file in ROOT TBrowser after completion.")
	runningOptionsGroup.add_argument("-b", "--batch", default=False, action="store_true",
	                                 help="Run with grid-control.")
	runningOptionsGroup.add_argument("-R", "--resume", default=False, action="store_true",
	                                 help="Resume the grid-control run and hadd after interrupting it.")
	
	
	if defaultExecutable: parser.add_argument("-x", "--executable", help="Artus executable. [Default: %s]" % str(defaultExecutable), default=defaultExecutable)
	else: parser.add_argument("-x", "--executable", help="Artus executable.", required=True)

	args = parser.parse_args()
	logger.initLogger(args)
	
	# input files check
	if len(args.input_files) == 1 and args.input_files[0] == "":
		args.input_files = []
	if all(map(lambda baseConfig: not baseConfig.endswith(".root"), args.base_configs)) and args.input_files == "":
		parser.error("No input file specified!")
	
	# work directory check
	args.work = os.path.expandvars(args.work)
	if not os.path.exists(args.work):
		os.makedirs(args.work)
	
	return args


# construct the JSON dictionary and return it
def constructJsonConfig(args):
	
	# initialise main JSON config
	mainConfig = jsonTools.JsonDict({ "OutputPath" : args.output_file })
	if len(args.input_files) > 0:
		mainConfig["InputFiles"] = reduce(lambda a, b: a+b, map(lambda inputFile: glob.glob(os.path.expandvars(inputFile)), args.input_files))
	
	# merge all base configs into the main config
	mainConfig += jsonTools.JsonDict.mergeAll(*args.base_configs)
	
	# treat pipeline configs
	pipelineJsonDict = {}
	if args.pipeline_configs and len(args.pipeline_configs) > 0:
		pipelineJsonDict = []
		for pipelineConfigs in args.pipeline_configs:
			pipelineJsonDict.append(jsonTools.JsonDict.expandAll(*map(lambda pipelineConfig: jsonTools.JsonDict.mergeAll(*pipelineConfig.split()), pipelineConfigs)))
		pipelineJsonDict = jsonTools.JsonDict.mergeAll(*pipelineJsonDict)
		pipelineJsonDict = jsonTools.JsonDict({ "Pipelines" : pipelineJsonDict })
	
	# merge resulting pipeline config into the main config
	mainConfig += jsonTools.JsonDict(pipelineJsonDict)
	
	# treat includes and comments
	mainConfig = mainConfig.doIncludes().doComments()
	
	return mainConfig


# run Artus analysis (C++ executable)
def runArtus(args, jsonConfig):
	exitCode = 0
	
	# save config file
	jsonConfigFileName = tempfile.mktemp(prefix="artus_", suffix=".json")
	jsonConfig.save(jsonConfigFileName)
	logging.getLogger(__name__).info("Saved JSON config \"%s\" for temporary usage." % jsonConfigFileName)
	
	if args.batch:
	
		# run Artus with grid-control
		pass
	
	else:
	
		# call C++ executable locally
		command = args.executable + " " + jsonConfigFileName
		logging.getLogger(__name__).info("Execute \"%s\"." % command)
		exitCode = os.system(command)
		if exitCode != 0:
			logging.getLogger(__name__).error("Exit with code %s.\n\n" % exitCode)
			logging.getLogger(__name__).info("Dump configuration:\n")
			print jsonConfigFileName

	# remove tmp. config
	logging.getLogger(__name__).info("Remove temporary config file.")
	os.system("rm " + jsonConfigFileName)
	
	return exitCode

