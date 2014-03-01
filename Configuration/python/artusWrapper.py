
# -*- coding: utf-8 -*-

import argparse
import logging
import os
import sys
import tempfile

import Artus.Utility.logger as logger
import Artus.Configuration.jsonTools as jsonTools


def artusWrapper(defaultExecutable=None):
	parser = argparse.ArgumentParser(parents=[logger.loggingParser],
	                                 description="Wrapper for Artus executables. JSON configs can be file names pointing to JSON text files or Artus ROOT output files with saved configs or python statements that can be evaluated as dictionary. When JSON configs are merged, the first ones in the list have a higher priority than later ones. In the final config, all includes and comments are replaced accordingly.")

	parser.add_argument("-c", "--base-configs", nargs="+", required=True,
	                    help="JSON base configurations. All configs are merged.")
	parser.add_argument("-p", "--pipeline-configs", nargs="+", action="append",
	                    help="JSON pipeline configurations. Single entries (whitespace separated strings) are first merged. Then all entries are expanded to get all possible combinations. For each expansion, this option has to be used. Afterwards, all results are merged into the JSON base config.")
	
	parser.add_argument("-i", "--input-files", help="Input root files. Leave empty (\"\") if input files from root file should be taken.", nargs="+", required=True)
	parser.add_argument("-o", "--output-file", help="Output root file. [Default: output.root]", default="output.root")
	
	if defaultExecutable: parser.add_argument("-x", "--executable", help="Artus executable. [Default: %s]" % str(defaultExecutable), default=defaultExecutable)
	else: parser.add_argument("-x", "--executable", help="Artus executable.", required=True)

	args = parser.parse_args()
	logger.initLogger(args)
	
	if len(args.input_files) == 1 and args.input_files[0] == "":
		args.input_files = []
	if all(map(lambda baseConfig: not baseConfig.endswith(".root"), args.base_configs)) and args.input_files == "":
		logging.getLogger(__name__).critial("No input file specified!")
		sys.exit(1)
	
	# constuct main JSON config
	mainConfig = jsonTools.JsonDict({ "OutputPath" : args.output_file })
	if len(args.input_files) > 0:
		mainConfig["InputFiles"] = args.input_files
	
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
	
	mainConfigFileName = tempfile.mktemp(prefix="artus_", suffix=".json")
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

