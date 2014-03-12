
# -*- coding: utf-8 -*-

import argparse
import glob
import logging
import os
import sys
import tempfile
import hashlib
import json
import subprocess

import Artus.Utility.logger as logger
import Artus.Configuration.jsonTools as jsonTools


# main function
class ArtusWrapper(object):

	def __init__(self, executable=None):

		self._config = jsonTools.JsonDict()
		self._executable = executable

		self.parser = None
		#Load default argument parser
		self._init_argumentparser()
		#Parse command line arguments and return dict
		self._args = vars(self.parser.parse_args())

	def run(self):

		#Expand Config
		self.expandConfig()
		self.saveConfig()

		#Run Artus if desired
		if not self._args['no_run']:
			callExecutable(self._configFilename())

	def setInputFilenames(self, files):
		if isinstance(files, basestring):
			self._config['InputFiles'] = glob.glob(files)
		else:
			self._config['InputFiles'] = files

	def setOutputFilename(self, output_filename):
		self._config["OutputPath"] = output_filename

	def getConfig(self):
		return self._config

	def setConfig(self, config):
		self._config = config

	def saveConfig(self, filepath=None):
		"""Save Config to File"""
		if not filepath:
			basename = "artus_{0}.json".format(hashlib.md5(str(self._config)).hexdigest())
			filepath = os.path.join(tempfile.gettempdir(), basename)
		self._configFilename = filepath
		self._config.save(filepath)
	
	def expandConfig(self):

		# merge all base configs into the main config
		print self._args.get('base_configs')
		self._config += jsonTools.JsonDict.mergeAll(self._args.get('base_configs'))

		#Set Input Filenames
		if self._args.get('input_files'):
			self.setInputFilenames(self._args.get('input_files'))
		if self._args.get('output_file'):
			self.setOutputFilename(self._args.get('output_file'))

		# treat pipeline configs
		pipelineJsonDict = {}
		if self._args.get('pipeline_configs') and len(self._args.get('pipeline_configs')) > 0:
			pipelineJsonDict = []
			for pipelineConfigs in self._args.get('pipeline_configs'):
				pipelineJsonDict.append(jsonTools.JsonDict.expandAll(*map(lambda pipelineConfig: jsonTools.JsonDict.mergeAll(*pipelineConfig.split()), pipelineConfigs)))
			pipelineJsonDict = jsonTools.JsonDict.mergeAll(*pipelineJsonDict)
			pipelineJsonDict = jsonTools.JsonDict({"Pipelines": pipelineJsonDict})
		
		# merge resulting pipeline config into the main config
		self._config += jsonTools.JsonDict(pipelineJsonDict)
		
		# treat includes and comments
		self._config = self._config.doComments().doIncludes().doComments()


	def _init_argumentparser(self):
	
		self.parser = argparse.ArgumentParser(parents=[logger.loggingParser], fromfile_prefix_chars="@",
		                                       description="Wrapper for Artus executables. JSON configs can be file names pointing to JSON text files or Artus ROOT output files with saved configs or python statements that can be evaluated as dictionary. When JSON configs are merged, the first ones in the list have a higher priority than later ones. In the final config, all includes and comments are replaced accordingly.")

		fileOptionsGroup = self.parser.add_argument_group("File options")
		fileOptionsGroup.add_argument("-i", "--input-files", nargs="+", required=False,
	                               help="Input root files. Leave empty (\"\") if input files from root file should be taken.")
		fileOptionsGroup.add_argument("-o", "--output-file", default="output.root",
	                               help="Output root file. [Default: output.root]")
		fileOptionsGroup.add_argument("-w", "--work", default="$ARTUS_WORK_BASE",
	                               help="Work directory base. [Default: $ARTUS_WORK_BASE]")
		fileOptionsGroup.add_argument("-n", "--project-name",
	                               help="Name for this Artus project specifies the name of the work subdirectory.")

		configOptionsGroup = self.parser.add_argument_group("Config options")
		configOptionsGroup.add_argument("-c", "--base-configs", nargs="+", required=False, default={},
	                                 help="JSON base configurations. All configs are merged.")
		configOptionsGroup.add_argument("-p", "--pipeline-configs", nargs="+", action="append",
	                                 help="JSON pipeline configurations. Single entries (whitespace separated strings) are first merged. Then all entries are expanded to get all possible combinations. For each expansion, this option has to be used. Afterwards, all results are merged into the JSON base config.")
		configOptionsGroup.add_argument("-P", "--print-config", default=False, action="store_true",
	                                 help="Print out the JSON config before running Artus.")

		runningOptionsGroup = self.parser.add_argument_group("Running options")
		runningOptionsGroup.add_argument("--no-run", default=False, action="store_true",
	                                  help="Exit before running Artus to only check the configs.")
		runningOptionsGroup.add_argument("-r", "--root", default=False, action="store_true",
	                                  help="Open output file in ROOT TBrowser after completion.")
		runningOptionsGroup.add_argument("-b", "--batch", default=False, action="store_true",
	                                  help="Run with grid-control.")
		runningOptionsGroup.add_argument("-R", "--resume", default=False, action="store_true",
	                                  help="Resume the grid-control run and hadd after interrupting it.")

		if self._executable:
			self.parser.add_argument("-x", "--executable", help="Artus executable. [Default: %s]" % str(self._executable), default=self._executable)
		else:
			self.parser.add_argument("-x", "--executable", help="Artus executable.", required=True)
	


	def callExecutable(self):
		"""run Artus analysis (C++ executable)"""
	
		exitCode = 0
	
		if args['batch']:
			# check work directory
			args['work'] = os.path.expandvars(args.get('work'))
			if not os.path.exists(args.get('work')):
				os.makedirs(args['work'])
	
			# run Artus with grid-control
			pass
	
		else:
	
			# check output directory
			outputDir = os.path.dirname(args.output_file)
			if outputDir > 0 and not os.path.exists(outputDir):
				os.makedirs(outputDir)
	
			# call C++ executable locally
			command = [args.executable, jsonConfigFileName]
			logging.getLogger(__name__).info("Execute \"%s\"." % command)
			exitCode = subprocess.call(command)
			if exitCode != 0:
				logging.getLogger(__name__).error("Exit with code %s.\n\n" % exitCode)
				logging.getLogger(__name__).info("Dump configuration:\n")
				print jsonConfigFileName
	
		# remove tmp. config
		logging.getLogger(__name__).info("Remove temporary config file.")
		os.system("rm " + jsonConfigFileName)
	
		return exitCode
