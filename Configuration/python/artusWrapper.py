# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse
import glob
import os
import sys
import tempfile
import hashlib
import json
import subprocess

import Artus.Utility.tools as tools
import Artus.Configuration.jsonTools as jsonTools


class ArtusWrapper(object):

	def __init__(self, executable=None, userArgParsers=None):

		self._config = jsonTools.JsonDict()
		self._executable = executable

		self._parser = None
		#Load default argument parser
		self._initArgumentParser(userArgParsers)
		#Parse command line arguments and return dict
		self._args = self._parser.parse_args()
		logger.initLogger(self._args)

		# write repository revisions to the config
		if self._args.add_repo_versions:
			self.setRepositoryRevisions()

		#Expand Config
		self.expandConfig()
		
	def run(self):
	
		exitCode = 0
		
		# save final config
		self.saveConfig()
		if self._args.print_config:
			log.info(self._config)

		#Run Artus if desired
		if not self._args.no_run:
			exitCode = self.callExecutable()
		
		if exitCode < 256:
			return exitCode
		else:
			return 1 # Artus sometimes returns exit codes >255 that are not supported

	def setInputFilenames(self, files):
		if isinstance(files, basestring):
			files = [files,]
		self._config["InputFiles"] = reduce(lambda a, b: a+b, map(lambda inputFile: glob.glob(os.path.expandvars(inputFile)), files))

	def setOutputFilename(self, output_filename):
		self._config["OutputPath"] = output_filename
	
	# write repository revisions to the config
	def setRepositoryRevisions(self):
		# expand possible environment variables in paths
		if isinstance(self._args.repo_scan_base_dirs, basestring):
			self._args.repo_scan_base_dirs = [self._args.repo_scan_base_dirs]
		self._args.repo_scan_base_dirs = [os.path.expandvars(repoScanBaseDir) for repoScanBaseDir in self._args.repo_scan_base_dirs]
		
		# construct possible scan paths
		subDirWildcards = ["*/" * level for level in range(self._args.repo_scan_depth+1)]
		scanDirWildcards = [os.path.join(repoScanBaseDir, subDirWildcard) for repoScanBaseDir in self._args.repo_scan_base_dirs for subDirWildcard in subDirWildcards]
		
		# globbing and filter for directories
		scanDirs = tools.flattenList([glob.glob(scanDirWildcard) for scanDirWildcard in scanDirWildcards])
		scanDirs = [scanDir for scanDir in scanDirs if os.path.isdir(scanDir)]
		
		# key: directory to check type of repository
		# value: command to extract the revision
		repoVersionCommands = {
			".git" : "git rev-parse HEAD",
			".svn" : "svn info | grep \"Revision\" | awk '{print $2}'"
		}
		# loop over dirs and revision control systems and write revisions to the config dict
		for repoDir, currentRevisionCommand in repoVersionCommands.items():
			repoScanDirs = [os.path.join(scanDir, repoDir) for scanDir in scanDirs]
			repoScanDirs = [glob.glob(os.path.join(scanDir, repoDir)) for scanDir in scanDirs]
			repoScanDirs = tools.flattenList([glob.glob(os.path.join(scanDir, repoDir)) for scanDir in scanDirs])
			repoScanDirs = [os.path.abspath(os.path.join(repoScanDir, "..")) for repoScanDir in repoScanDirs]
			
			for repoScanDir in repoScanDirs:
				popenCout, popenCerr = subprocess.Popen(currentRevisionCommand.split(), stdout=subprocess.PIPE, cwd=repoScanDir).communicate()
				self._config[repoScanDir] = popenCout.replace("\n", "")

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
		log.info("Saved JSON config \"%s\" for temporary usage." % self._configFilename)
	
	def expandConfig(self):

		# merge all base configs into the main config
		self._config += jsonTools.JsonDict.mergeAll(self._args.base_configs)

		#Set Input Filenames
		if self._args.input_files:
			self.setInputFilenames(self._args.input_files)
		if self._args.output_file:
			self.setOutputFilename(self._args.output_file)

		# treat pipeline configs
		pipelineJsonDict = {}
		if self._args.pipeline_configs and len(self._args.pipeline_configs) > 0:
			pipelineJsonDict = []
			for pipelineConfigs in self._args.pipeline_configs:
				pipelineJsonDict.append(jsonTools.JsonDict.expandAll(*map(lambda pipelineConfig: jsonTools.JsonDict.mergeAll(*pipelineConfig.split()), pipelineConfigs)))
			pipelineJsonDict = jsonTools.JsonDict.mergeAll(*pipelineJsonDict)
			pipelineJsonDict = jsonTools.JsonDict({"Pipelines": pipelineJsonDict})
		
		# merge resulting pipeline config into the main config
		self._config += jsonTools.JsonDict(pipelineJsonDict)
		
		# treat includes
		self._config = self._config.doIncludes()


	def _initArgumentParser(self, userArgParsers=None):

		if userArgParsers is None:
			userArgParsers = []

		self._parser = argparse.ArgumentParser(parents=[logger.loggingParser] + userArgParsers, fromfile_prefix_chars="@",
		                                       description="Wrapper for Artus executables. JSON configs can be file names pointing to JSON text files or Artus ROOT output files with saved configs or python statements that can be evaluated as dictionary. When JSON configs are merged, the first ones in the list have a higher priority than later ones. In the final config, all includes and comments are replaced accordingly.")

		fileOptionsGroup = self._parser.add_argument_group("File options")
		fileOptionsGroup.add_argument("-i", "--input-files", nargs="+", required=False,
	                               help="Input root files. Leave empty (\"\") if input files from root file should be taken.")
		fileOptionsGroup.add_argument("-o", "--output-file", default="output.root",
	                               help="Output root file. [Default: output.root]")
		fileOptionsGroup.add_argument("-w", "--work", default="$ARTUS_WORK_BASE",
	                               help="Work directory base. [Default: $ARTUS_WORK_BASE]")
		fileOptionsGroup.add_argument("-n", "--project-name",
	                               help="Name for this Artus project specifies the name of the work subdirectory.")

		configOptionsGroup = self._parser.add_argument_group("Config options")
		configOptionsGroup.add_argument("-c", "--base-configs", nargs="+", required=False, default={},
	                                 help="JSON base configurations. All configs are merged.")
		configOptionsGroup.add_argument("-p", "--pipeline-configs", nargs="+", action="append",
	                                 help="JSON pipeline configurations. Single entries (whitespace separated strings) are first merged. Then all entries are expanded to get all possible combinations. For each expansion, this option has to be used. Afterwards, all results are merged into the JSON base config.")
		configOptionsGroup.add_argument("--add-repo-versions", default=True, action="store_true",
	                                 help="Add repository versions to the JSON config.")
		configOptionsGroup.add_argument("--repo-scan-base-dirs", nargs="+", required=False, default="$CMSSW_BASE/src/",
	                                 help="Base directories for repositories scran. [Default: $CMSSW_BASE/src/]")
		configOptionsGroup.add_argument("--repo-scan-depth", required=False, type=int, default=2,
	                                 help="Depth of repositories scran. [Default: 2")
		configOptionsGroup.add_argument("-P", "--print-config", default=False, action="store_true",
	                                 help="Print out the JSON config before running Artus.")

		runningOptionsGroup = self._parser.add_argument_group("Running options")
		runningOptionsGroup.add_argument("--no-run", default=False, action="store_true",
	                                  help="Exit before running Artus to only check the configs.")
		runningOptionsGroup.add_argument("-r", "--root", default=False, action="store_true",
	                                  help="Open output file in ROOT TBrowser after completion.")
		runningOptionsGroup.add_argument("-b", "--batch", default=False, action="store_true",
	                                  help="Run with grid-control.")
		runningOptionsGroup.add_argument("-R", "--resume", default=False, action="store_true",
	                                  help="Resume the grid-control run and hadd after interrupting it.")

		if self._executable:
			self._parser.add_argument("-x", "--executable", help="Artus executable. [Default: %s]" % str(self._executable), default=self._executable)
		else:
			self._parser.add_argument("-x", "--executable", help="Artus executable.", required=True)
	


	def callExecutable(self):
		"""run Artus analysis (C++ executable)"""
	
		exitCode = 0
	
		if self._args.batch:
			# check work directory
			self._args.work = os.path.expandvars(self._args.work)
			if not os.path.exists(self._args.work):
				os.makedirs(self._args.work)
	
			# run Artus with grid-control
			pass
	
		else:
	
			# check output directory
			outputDir = os.path.dirname(self._args.output_file)
			if outputDir and not os.path.exists(outputDir):
				os.makedirs(outputDir)
	
			# call C++ executable locally
			command = self._executable + " " + self._configFilename
			log.info("Execute \"%s\"." % command)
			exitCode = logger.subprocessCall(command.split())
			
			if exitCode != 0:
				log.error("Exit with code %s.\n\n" % exitCode)
				log.info("Dump configuration:\n")
				log.info(self._configFilename) # TODO
	
		# remove tmp. config
		# logging.getLogger(__name__).info("Remove temporary config file.")
		# os.system("rm " + self._configFilename)
	
		return exitCode
