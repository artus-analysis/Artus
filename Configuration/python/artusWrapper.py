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
import re
from string import Template
from datetime import datetime

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
		
		# expand the environment variables only at the batch node
		if self._args.batch:
			self._args.envvar_expansion = False

		date_now = datetime.now().strftime("%Y-%m-%d_%H-%M")

		# write repository revisions to the config
		if not self._args.disable_repo_versions:
			self.setRepositoryRevisions()
			self._config["Date"] = date_now

		#Expand Config
		self.expandConfig()
		if self._args.batch:
			self.projectPath = os.path.join(os.path.expandvars(self._args.work), date_now+"_"+self._args.project_name)

	def run(self):
	
		exitCode = 0
		
		# save final config
		if self._args.save_config:
			self.saveConfig(self._args.save_config)
		elif self._args.batch:
			basename = "artus_{0}.json".format(hashlib.md5(str(self._config)).hexdigest())
			filepath = os.path.join(self.projectPath, basename)
			if not os.path.exists(self.projectPath):
				os.makedirs(self.projectPath)
				os.makedirs(os.path.join(self.projectPath, "output"))
			self.saveConfig(filepath)
		else:
			self.saveConfig()

		if self._args.print_config:
			log.info(self._config)

		#Run Artus if desired
		if self._args.batch:
			exitCode = self.sendToBatchSystem()
		elif not self._args.no_run:
			exitCode = self.callExecutable()


		if exitCode < 256:
			return exitCode
		else:
			return 1 # Artus sometimes returns exit codes >255 that are not supported

	def setInputFilenames(self, filelist, alreadyInGridControl = False):
		if (not (isinstance(self._config["InputFiles"], list)) and not isinstance(self._config["InputFiles"], basestring)):
			self._config["InputFiles"] = []
		for entry in filelist:
			if os.path.splitext(entry)[1] == ".root":
				if entry.find("*") != -1:
					filelist = glob.glob(os.path.expandvars(entry))
					self._gridControlInputFiles.append(self.extractNickname(filelist[0]) + ":scan:" + entry)
					self.setInputFilenames(filelist, True)
				else:
					self._config["InputFiles"].append(entry)
					if not alreadyInGridControl:
						self._gridControlInputFiles.append(self.extractNickname(entry) + ":file:" + entry + "|1")
			elif os.path.isdir(entry):
				self.setInputFilenames([os.path.join(entry, "*.root")])
			elif (os.path.splitext(entry))[1] == ".txt":
				txtFile = open(entry, 'r')
				txtFileContent = txtFile.readlines()
				for line in range(len(txtFileContent)):
					txtFileContent[line] = txtFileContent[line].replace("\n", "")
				txtFile.close()
				self.setInputFilenames(txtFileContent)
			else:
				log.debug("Found file in input search path that is not further considered: " + entry + "\n")

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
		self._config = jsonTools.JsonDict(config)

	def removeUnwantedInputFiles(self):
		if self._args.batch:  # shrink config by inputFiles since this is replaced anyway in batch mode
			self._config["InputFiles"] = [""]
		elif self._args.fast:
			self._config["InputFiles"] = self._config["InputFiles"][:min(len(self._config["InputFiles"]), self._args.fast)]

	def saveConfig(self, filepath=None):
		"""Save Config to File"""
		if not filepath:
			basename = "artus_{0}.json".format(hashlib.md5(str(self._config)).hexdigest())
			filepath = os.path.join(tempfile.gettempdir(), basename)
		self._configFilename = filepath
		self._config.save(filepath, indent=4)
		log.info("Saved JSON config \"%s\" for temporary usage." % self._configFilename)


	def expandConfig(self):

		# merge all base configs into the main config
		self._config += jsonTools.JsonDict.mergeAll(self._args.base_configs)
		self._gridControlInputFiles = []

		#Set Input Filenames
		if self._args.input_files:
			self._config["InputFiles"] = [] #overwrite settings from config file by command line
			inputFileList = self._args.input_files
			for entry in range(len(inputFileList)):
				inputFileList[entry] = inputFileList[entry].replace('"', '').replace("'", '').replace(',', '')
			self.setInputFilenames(self._args.input_files)
		else:
			tmpInputFiles = self._config["InputFiles"]
			self._config["InputFiles"] = []
			self.setInputFilenames(tmpInputFiles)

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
		pipelineJsonDict = jsonTools.JsonDict(pipelineJsonDict)
		
		# treat pipeline base configs
		pipelineBaseJsonDict = jsonTools.JsonDict()
		if self._args.pipeline_base_configs and len(self._args.pipeline_base_configs) > 0:
			pipelineBaseJsonDict = jsonTools.JsonDict({
				"Pipelines" : {
					pipeline : jsonTools.JsonDict(*self._args.pipeline_base_configs) for pipeline in pipelineJsonDict["Pipelines"].keys()
				}
			})
		
		# merge resulting pipeline config into the main config
		self._config += (pipelineBaseJsonDict + pipelineJsonDict)

		# shrink Input Files to requested Number
		self.removeUnwantedInputFiles()

		# treat includes, nicks and comments
		nickname = self.determineNickname(self._args.nick)
		self._config = self._config.doIncludes().doNicks(nickname).doComments()
		
		# treat environment variables
		if self._args.envvar_expansion:
			self._config = self._config.doExpandvars()
		
		# set log level
		self._config["LogLevel"] = self._args.log_level

	def determineNickname(self, nickname):
		if nickname.find("auto") != -1: # automatic determination of nicknames
			nickname = self.extractNickname(self._config["InputFiles"][0])
			for path in self._config["InputFiles"]:
				tmpNick = self.extractNickname(path)
				if tmpNick != nickname:
					if not self._args.batch:
						log.fatal("Input files do have different nicknames, which would cause errors. Aborting.")
						sys.exit(1)
		return nickname

	def extractNickname(self, string):
		filename = os.path.basename(string)
		nickname = filename[filename.find("_")+1:filename.rfind("_")]
		return nickname


	def replaceLines(self, inputList, replacingDict):
		for line in range(len(inputList)):
			inputList[line] = Template(inputList[line]).safe_substitute(replacingDict)


	def _initArgumentParser(self, userArgParsers=None):

		if userArgParsers is None:
			userArgParsers = []

		self._parser = argparse.ArgumentParser(parents=[logger.loggingParser] + userArgParsers, fromfile_prefix_chars="@",
		                                       description="Wrapper for Artus executables. JSON configs can be file names pointing to JSON text files or Artus ROOT output files with saved configs or python statements that can be evaluated as dictionary. When JSON configs are merged, the first ones in the list have a higher priority than later ones. In the final config, all includes and comments are replaced accordingly.")

		fileOptionsGroup = self._parser.add_argument_group("File options")
		fileOptionsGroup.add_argument("-i", "--input-files", nargs="+", required=False,
	                               help="Input root files. Leave empty (\"\") if input files from root file should be taken.")
		fileOptionsGroup.add_argument("-o", "--output-file", default="output.root",
	                               help="Output root file. [Default: %(default)s]")
		fileOptionsGroup.add_argument("-w", "--work", default="$ARTUS_WORK_BASE",
	                               help="Work directory base. [Default: %(default)s]")
		fileOptionsGroup.add_argument("-n", "--project-name", default="analysis",
	                               help="Name for this Artus project specifies the name of the work subdirectory.")

		configOptionsGroup = self._parser.add_argument_group("Config options")
		configOptionsGroup.add_argument("-c", "--base-configs", nargs="+", required=False, default={},
	                                 help="JSON base configurations. All configs are merged.")
		configOptionsGroup.add_argument("-C", "--pipeline-base-configs", nargs="+",
	                                 help="JSON pipeline base configurations. All pipeline configs will be merged with these common configs.")
		configOptionsGroup.add_argument("-p", "--pipeline-configs", nargs="+", action="append",
	                                 help="JSON pipeline configurations. Single entries (whitespace separated strings) are first merged. Then all entries are expanded to get all possible combinations. For each expansion, this option has to be used. Afterwards, all results are merged into the JSON base config.")
		configOptionsGroup.add_argument("--nick", default="auto",
	                                    help="Kappa nickname name that can be used for switch between sample-dependent settings.")
		
		configOptionsGroup.add_argument("--disable-repo-versions", default=False, action="store_true",
	                                 help="Add repository versions to the JSON config.")
		configOptionsGroup.add_argument("--repo-scan-base-dirs", nargs="+", required=False, default="$CMSSW_BASE/src/",
	                                 help="Base directories for repositories scan. [Default: $CMSSW_BASE/src/]")
		configOptionsGroup.add_argument("--repo-scan-depth", required=False, type=int, default=2,
	                                 help="Depth of repositories scran. [Default: %(default)s]")
		configOptionsGroup.add_argument("--enable-envvar-expansion", dest="envvar_expansion", default=True, action="store_true",
	                                 help="Enable expansion of environment variables in config.")
		configOptionsGroup.add_argument("--disable-envvar-expansion", dest="envvar_expansion", action="store_false",
	                                 help="Disable expansion of environment variables in config.")
		configOptionsGroup.add_argument("-P", "--print-config", default=False, action="store_true",
	                                 help="Print out the JSON config before running Artus.")
		configOptionsGroup.add_argument("-s", "--save-config", default="",
	                                 help="Save the JSON config to FILENAME.")
		configOptionsGroup.add_argument('-f', '--fast', type=int, default=False,
	                                 help="limit number of input files or grid-control jobs. 3=files[0:3].")
		configOptionsGroup.add_argument("--gc-config", default="$ARTUSPATH/Configuration/data/grid-control_base_config.conf",
	                                 help="path to grid-control base config that is replace by the wrapper. [Default: %(default)s]")


		runningOptionsGroup = self._parser.add_argument_group("Running options")
		runningOptionsGroup.add_argument("--no-run", default=False, action="store_true",
	                                  help="Exit before running Artus to only check the configs.")
		runningOptionsGroup.add_argument("-r", "--root", default=False, action="store_true",
	                                  help="Open output file in ROOT TBrowser after completion.")
		runningOptionsGroup.add_argument("-b", "--batch", default=False, action="store_true",
	                                  help="Run with grid-control.")
		runningOptionsGroup.add_argument("--no-log-to-se", default=False, action="store_true",
	                                  help="Do not write logfile in batch mode directly to SE.")

		if self._executable:
			self._parser.add_argument("-x", "--executable", help="Artus executable. [Default: %(default)s]", default=self._executable)
		else:
			self._parser.add_argument("-x", "--executable", help="Artus executable. [Default: %(default)s]", required=True)

	def sendToBatchSystem(self):

		gcConfigFilePath = os.path.expandvars(self._args.gc_config)
		gcConfigFile = open(gcConfigFilePath,"r")
		tmpGcConfigFileBasename = "grid-control_base_config_{0}.conf".format(hashlib.md5(str(self._config)).hexdigest())
		tmpGcConfigFileBasepath = os.path.join(self.projectPath, tmpGcConfigFileBasename)

		# open base file and save it to a list
		tmpGcConfigFile = open(tmpGcConfigFileBasepath,"w")
		gcConfigFileContent = gcConfigFile.readlines()
		gcConfigFile.close()

		# modify base file
		datasetString = ""
		for inputEntry in self._gridControlInputFiles:
			datasetString += "\t" + inputEntry + "\n"

		epilogArguments  = r"epilog arguments = "
		epilogArguments += r"--disable-repo-versions "
		epilogArguments += r"--log-level debug "
		if self._args.no_log_to_se:
			epilogArguments += r"--log-files log.txt "
		else:
			epilogArguments += r"--log-files " + os.path.join(self.projectPath, "output/") + "${DATASETNICK}_job_${MY_JOBID}_log.txt "
		epilogArguments += r"-c " + os.path.basename(self._configFilename) + " "
		epilogArguments += "--nick $DATASETNICK "
		epilogArguments += '-i $FILE_NAMES '

		sepathRaw = os.path.join(self.projectPath, "output")
		sepath = "se path = " + sepathRaw
		workdir = "workdir = " + os.path.join(self.projectPath, "workdir")

		replacingDict = dict( epilogexecutable = "epilog executable = $CMSSW_BASE/bin/" + os.path.join(os.path.expandvars("$SCRAM_ARCH"), os.path.basename(sys.argv[0])),
		                      sepath = sepath,
		                      workdir = workdir,
		                      jobs= "" if not self._args.fast else "jobs = " + str(self._args.fast),
		                      inputfiles= "input files = \n\t" + self._configFilename,
		                      dataset = "dataset = \n " + datasetString,
		                      epilogarguments = epilogArguments,
		                      seoutputfiles = "se output files = *.txt *.root" if self._args.no_log_to_se else "se output files = *.root"
		                      )

		self.replaceLines(gcConfigFileContent, replacingDict)

		# save it
		for line in gcConfigFileContent:
			tmpGcConfigFile.write(line)
		tmpGcConfigFile.close()

		exitCode = 0
		command = "go.py " + tmpGcConfigFileBasepath
		log.info("Execute \"%s\"." % command)
		if not self._args.no_run:
			exitCode = logger.subprocessCall(command.split())
		
		log.info("Output is written to directory \"%s\"" % sepathRaw)

		if exitCode != 0:
			log.error("Exit with code %s.\n\n" % exitCode)
			log.info("Dump configuration:\n")
			log.info(self._configFilename)

		return exitCode


	def callExecutable(self):
		"""run Artus analysis (C++ executable)"""
		exitCode = 0

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
