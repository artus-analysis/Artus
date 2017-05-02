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
import shutil
import subprocess
import re
from string import Template
from datetime import datetime

import Artus.Utility.tools as tools
import Artus.Utility.jsonTools as jsonTools
import Artus.Utility.profile_cpp as profile_cpp


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

		# write username to the config
		try:
			self._config["User"] = os.environ["USER"] 
		except:
			import random
			import string
			self._config["User"] = ''.join(random.choice(string.ascii_uppercase + string.digits) for _ in range(10))

		#Expand Config
		self.expandConfig()
		self.projectPath = None
		self.localProjectPath = None
		self.remote_se = False

		if self._args.batch:
			self.projectPath = os.path.join(os.path.expandvars(self._args.work), date_now+"_"+self._args.project_name)
			self.localProjectPath = self.projectPath
			if self.projectPath.startswith("srm://"):
				self.remote_se = True
				self.localProjectPath = os.path.join(os.path.expandvars(self._parser.get_default("work")), date_now+"_"+self._args.project_name)

	def run(self):

		exitCode = 0

		# save final config
		if self._args.save_config:
			self.saveConfig(self._args.save_config)
		elif self._args.batch:
			basename = "artus_config.json"
			filepath = os.path.join(self.localProjectPath, basename)
			if not os.path.exists(self.localProjectPath):
				os.makedirs(self.localProjectPath)
				os.makedirs(os.path.join(self.localProjectPath, "output"))
			self.saveConfig(filepath)
		else:
			self.saveConfig()

		if self._args.print_config:
			log.info(self._config)

		# set LD_LIBRARY_PATH
		if not self._args.ld_library_paths is None:
			for path in self._args.ld_library_paths:
				if path not in os.environ.get("LD_LIBRARY_PATH", ""):
					os.environ["LD_LIBRARY_PATH"] = path+":"+os.environ.get("LD_LIBRARY_PATH", "")

		# print environment variables
		if self._args.print_envvars:
			for envvar in self._args.print_envvars:
				log.info("$%s = %s" % (envvar, os.environ.get(envvar, "")))

		#Run Artus if desired
		if self._args.profile:
			exitCode = self.measurePerformance(self._args.profile)
		elif self._args.batch:
			exitCode = self.sendToBatchSystem()
		elif not self._args.no_run:
			exitCode = self.callExecutable()
		
		if (not self.tmp_directory_remote_files is None):
			shutil.rmtree(self.tmp_directory_remote_files)
		
		if exitCode < 256:
			return exitCode
		else:
			return 1 # Artus sometimes returns exit codes >255 that are not supported

	def readDbsFile(self, path):
		dbsInput = {}
		with open(path, "r") as dbsfile:
			key = ""
			for line in dbsfile:
				if "[" in line:
					key = line.strip().replace("[", "").replace("]", "")
					dbsInput[key] = []
				elif "=" in line:
					first, second = line.split("=")
					try:
						float(second)
					except ValueError:
						continue
					dbsInput[key].append(first.strip())
		return dbsInput

	def removeProcessedFiles(self, dbs, path):
		#import pdb
		base_path, trash = os.path.split(path)
		base_path = os.path.join(base_path, "workdir")
		job_list = glob.glob(os.path.join(base_path, "jobs/job_*.txt"))
		for job in job_list:
			with open(job, "r") as jobinfo:
				parse_files = False
				for line in jobinfo:
					var, status = line.strip().split("=")
					status = status.replace('"', '')
					if "status" in line and "SUCCESS" in line:
						parse_files = True
						break
			if parse_files:
				job_gc = job.replace("/jobs/", "/output/").replace(".txt", "/gc.stdout")
				files = ""
				with open(job_gc, "r") as gcfile:
					for line in gcfile:
						if "export FILE_NAMES" in line:
							var, files = line.split("=")
							files = files.strip().replace('\\', '').replace('"', '')
							files = files.split(", ")
							break
				main_key = ""
				for key in dbs.keys():
					#pdb.set_trace()
					if re.search("kappa_%s_[0-9]+.root"%key, files[0]):
						main_key = key
						break
				for sfile in files:
					try:
						ind = dbs[main_key].index(sfile)
						dbs[main_key].pop(ind)
					except ValueError:
						continue
		length = 0
		for key, item in dbs.iteritems():
			length += len(item)
		log.info("Final dbs consists of %i files" %length)
		return dbs
	
	def setInputFilenames(self, filelist, alreadyInGridControl = False):
		if (not (isinstance(self._config["InputFiles"], list)) and not isinstance(self._config["InputFiles"], basestring)):
			self._config["InputFiles"] = []
		for entry in filelist:
			if os.path.splitext(entry)[1] == ".root":
				if entry.find("*") != -1:
					filelist = glob.glob(os.path.expandvars(entry))
					self.setInputFilenames(filelist, alreadyInGridControl)
				else:
					self._config["InputFiles"].append(entry)
					if not alreadyInGridControl:
						self._gridControlInputFiles.setdefault(self.extractNickname(entry), []).append(entry + " = 1")
			elif os.path.splitext(entry)[1] == ".dbs":
				tmpDBS = self.readDbsFile(entry)
				tmpDBS = self.removeProcessedFiles(tmpDBS, entry)
				filelist = []
				for key,item in tmpDBS.iteritems():
					filelist += item
				self.setInputFilenames(filelist, alreadyInGridControl)
			elif os.path.isdir(entry):
				self.setInputFilenames([os.path.join(entry, "*.root")])
			elif (os.path.splitext(entry))[1] == ".txt":
				txtFile = open(os.path.expandvars(entry), 'r')
				txtFileContent = txtFile.readlines()
				for line in range(len(txtFileContent)):
					txtFileContent[line] = txtFileContent[line].replace("\n", "")
				txtFile.close()
				self.setInputFilenames(txtFileContent)
			else:
				log.warning("Found file in input search path that is not further considered: " + entry + "\n")

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
			".svn" : "svn info"# | grep Revision | awk '{print $2}'"
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
		elif not self._args.fast is None:
			self._config["InputFiles"] = self._config["InputFiles"][:min(len(self._config["InputFiles"]), self._args.fast)]

	def remove_pipeline_copies(self):
		pipelines = self._config.get("Pipelines", {}).keys()
		pipelines_to_remove = []
		pipeline_renamings = {}
		for index1, pipeline1 in enumerate(pipelines):
			if pipeline1 in pipelines_to_remove:
				continue

			for pipeline2 in pipelines[index1+1:]:
				if pipeline2 in pipelines_to_remove:
					continue

				difference = jsonTools.JsonDict.deepdiff(self._config["Pipelines"][pipeline1],
				                                         self._config["Pipelines"][pipeline2])
				if len(difference[0]) == 0 and len(difference[1]) == 0:
					pipelines_to_remove.append(pipeline2)
					new_name = tools.find_common_string(pipeline_renamings.get(pipeline1, pipeline1),
					                                    pipeline_renamings.get(pipeline2, pipeline2))
					pipeline_renamings[pipeline1] = new_name.strip("_").replace("__", "_")

		for pipeline in pipelines_to_remove:
			self._config["Pipelines"].pop(pipeline)

		for old_name, new_name in pipeline_renamings.iteritems():
			self._config["Pipelines"][new_name] = self._config["Pipelines"].pop(old_name)

	def saveConfig(self, filepath=None):
		"""Save Config to File"""
		if not filepath:
			basename = "artus_{0}.json".format(hashlib.md5(str(self._config)).hexdigest())
			filepath = os.path.join(tempfile.gettempdir(), basename)
		self._configFilename = filepath
		self._config.save(filepath, indent=4)
		log.info("Saved JSON config \"%s\" for temporary usage." % self._configFilename)

	def readInExternals(self):
		# to be overwritten by users
		pass

	def expandConfig(self):

		# merge all base configs into the main config
		self._config += jsonTools.JsonDict.mergeAll(self._args.base_configs)
		self._gridControlInputFiles = {}

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

		if not self._args.n_events is None:
			self._config["ProcessNEvents"] = self._args.n_events

		# shrink Input Files to requested Number
		self.removeUnwantedInputFiles()

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

		# treat includes, nicks and comments
		self.tmp_directory_remote_files = None
		if self._args.batch:
			self._config = self._config.doIncludes().doComments()
			self._config["BatchMode"] = True
		else:
			nickname = self.determineNickname(self._args.nick)
			self._config = self._config.doIncludes().doNicks(nickname).doComments()
			self._config["Nickname"] = nickname

			#read in external values
			self.readInExternals()

		# remove all but one of similar pipeline copies
		self.remove_pipeline_copies()

		# treat environment variables
		if self._args.envvar_expansion:
			self._config = self._config.doExpandvars()
		
		# treat remote files
		if self._args.copy_remote_files:
			self.useLocalCopiesOfRemoteFiles()

		# set log level
		self._config["LogLevel"] = self._args.log_level

	def determineNickname(self, nickname):
		if nickname.find("auto") != -1: # automatic determination of nicknames
			nickname = self.extractNickname(self._config["InputFiles"][0])
			for path in self._config["InputFiles"]:
				tmpNick = self.extractNickname(path)
				if tmpNick != nickname:
					if not self._args.batch:
						log.warning("Input files do have different nicknames, which could cause errors.")
		return nickname

	def extractNickname(self, string):
		filename = os.path.basename(string)
		nickname = filename[filename.find("_")+1:filename.rfind("_")]
		# in case nickname extraction above failes, use the one imposed by --nick; default: "auto"
		if nickname == "":
			nickname = self._args.nick
		return nickname

	def replaceLines(self, inputList, replacingDict):
		for line in range(len(inputList)):
			inputList[line] = Template(inputList[line]).safe_substitute(replacingDict)

	def useLocalCopiesOfRemoteFiles(self, remote_identifiers=None):
		if remote_identifiers is None:
			remote_identifiers = ["dcap", "root"]
		
		self.tmp_directory_remote_files = tempfile.mkdtemp(prefix="artus_remote_files_")
		self._config = self._config.doReplaceFilesByLocalCopies(self.tmp_directory_remote_files, remote_identifiers)

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
		configOptionsGroup.add_argument("--repo-scan-depth", required=False, type=int, default=3,
		                                help="Depth of repositories scran. [Default: %(default)s]")
		configOptionsGroup.add_argument("--enable-envvar-expansion", dest="envvar_expansion", default=True, action="store_true",
		                                help="Enable expansion of environment variables in config.")
		configOptionsGroup.add_argument("--disable-envvar-expansion", dest="envvar_expansion", action="store_false",
		                                help="Disable expansion of environment variables in config.")
		configOptionsGroup.add_argument("-P", "--print-config", default=False, action="store_true",
		                                help="Print out the JSON config before running Artus.")
		configOptionsGroup.add_argument("--print-envvars", nargs="+",
		                                help="Log specified environment variables.")
		configOptionsGroup.add_argument("-s", "--save-config", default="",
		                                help="Save the JSON config to FILENAME.")
		configOptionsGroup.add_argument("-f", "--fast", type=int,
		                                help="Limit number of input files or grid-control jobs. 3=files[0:3].")
		configOptionsGroup.add_argument("-e", "--n-events", type=int,
		                                help="Limit number of events to process.")
		configOptionsGroup.add_argument("--gc-config", default="$CMSSW_BASE/src/Artus/Configuration/data/grid-control_base_config.conf",
		                                help="Path to grid-control base config that is replace by the wrapper. [Default: %(default)s]")
		configOptionsGroup.add_argument("--gc-config-includes", nargs="+",
		                                help="Path to grid-control configs to include in the base config.")

		runningOptionsGroup = self._parser.add_argument_group("Running options")
		runningOptionsGroup.add_argument("--no-run", default=False, action="store_true",
		                                 help="Exit before running Artus to only check the configs.")
		runningOptionsGroup.add_argument("--copy-remote-files", default=False, action="store_true",
		                                 help="Copy remote files first to avoid too many open connections.")
		runningOptionsGroup.add_argument("--ld-library-paths", nargs="+",
		                                 help="Add paths to environment variable LD_LIBRARY_PATH.")
		runningOptionsGroup.add_argument("--profile", default="",
		                                 help="Measure performance with profiler. Choose igprof or valgrind.")
		runningOptionsGroup.add_argument("-r", "--root", default=False, action="store_true",
		                                 help="Open output file in ROOT TBrowser after completion.")
		runningOptionsGroup.add_argument("-b", "--batch", default=False, const="naf", nargs="?",
		                                 help="Run with grid-control. Optionally select backend. [Default: %(default)s]")
		runningOptionsGroup.add_argument("--files-per-job", type=int, default=15,
		                                 help="Files per batch job. [Default: %(default)s]")
		runningOptionsGroup.add_argument("--area-files", default=None,
		                                 help="Additional area files. [Default: %(default)s]")
		runningOptionsGroup.add_argument("--wall-time", default="24:00:00",
		                                 help="Wall time of batch jobs. [Default: %(default)s]")
		runningOptionsGroup.add_argument("--memory", type=int, default=3000,
		                                 help="Memory (in MB) for batch jobs. [Default: %(default)s]")
		runningOptionsGroup.add_argument("--cmdargs", default="-cG -m 3",
		                                 help="Command line arguments for go.py. [Default: %(default)s]")
		runningOptionsGroup.add_argument("--se-path",
		                                 help="Custom SE path, if it should different from the work directory.")
		runningOptionsGroup.add_argument("--log-to-se", default=False, action="store_true",
		                                 help="Write logfile in batch mode directly to SE. Does not work with remote batch system")
		runningOptionsGroup.add_argument("--partition-lfn-modifier", default = None,
		                                 help="Forces a certain access to input files. See base conf for corresponding dictionary")
		                                 

		if self._executable:
			self._parser.add_argument("-x", "--executable", help="Artus executable. [Default: %(default)s]", default=self._executable)
		else:
			self._parser.add_argument("-x", "--executable", help="Artus executable. [Default: %(default)s]", required=True)

	def sendToBatchSystem(self):

		# write dbs file
		dbsFileContent = tools.write_dbsfile(self._gridControlInputFiles)

		dbsFileBasename = "datasets.dbs"
		dbsFileBasepath = os.path.join(self.localProjectPath, dbsFileBasename)
		with open(dbsFileBasepath, "w") as dbsFile:
			dbsFile.write(dbsFileContent)

		gcConfigFilePath = os.path.expandvars(self._args.gc_config)
		gcConfigFile = open(gcConfigFilePath,"r")
		tmpGcConfigFileBasename = "grid-control_config.conf"
		tmpGcConfigFileBasepath = os.path.join(self.localProjectPath, tmpGcConfigFileBasename)

		# open base file and save it to a list
		tmpGcConfigFile = open(tmpGcConfigFileBasepath,"w")
		gcConfigFileContent = gcConfigFile.readlines()
		gcConfigFile.close()

		sepathRaw = os.path.join(self.projectPath, "output")

		epilogArguments  = r"epilog arguments = "
		epilogArguments += r"--disable-repo-versions "
		epilogArguments += r"--log-level " + self._args.log_level + " "
		if self._args.log_to_se:
			epilogArguments += r"--log-files " + os.path.join(sepathRaw, "${DATASETNICK}", "${DATASETNICK}_job_${MY_JOBID}_log.log") + " "
		else:
			epilogArguments += r"--log-files log.log --log-stream stdout "
		epilogArguments += r"--print-envvars ROOTSYS CMSSW_BASE DATASETNICK FILE_NAMES LD_LIBRARY_PATH "
		epilogArguments += r"-c " + os.path.basename(self._configFilename) + " "
		epilogArguments += "--nick $DATASETNICK "
		epilogArguments += "-i $FILE_NAMES --copy-remote-files"
		if not self._args.ld_library_paths is None:
			epilogArguments += ("--ld-library-paths %s" % " ".join(self._args.ld_library_paths))

		sepath = "se path = " + (self._args.se_path if self._args.se_path else sepathRaw)
		workdir = "workdir = " + os.path.join(self.localProjectPath, "workdir")
		backend = open(os.path.expandvars("$CMSSW_BASE/src/Artus/Configuration/data/grid-control_backend_" + self._args.batch + ".conf"), 'r').read()
		self.replacingDict = dict(
				include = ("include = " + " ".join(self._args.gc_config_includes) if self._args.gc_config_includes else ""),
				epilogexecutable = "epilog executable = " + os.path.basename(sys.argv[0]),
				sepath = sepath,
				workdir = workdir,
				jobs = "" if self._args.fast is None else "jobs = " + str(self._args.fast),
				inputfiles = "input files = \n\t" + self._configFilename + "\n\t" + os.path.expandvars(os.path.join("$CMSSW_BASE/bin/$SCRAM_ARCH", os.path.basename(sys.argv[0]))),
				filesperjob = "files per job = " + str(self._args.files_per_job),
				areafiles = self._args.area_files if (self._args.area_files != None) else "",
				walltime = "wall time = " + self._args.wall_time,
				memory = "memory = " + str(self._args.memory),
				cmdargs = "cmdargs = " + self._args.cmdargs,
				dataset = "dataset = \n\t:ListProvider:" + dbsFileBasepath,
				epilogarguments = epilogArguments,
				seoutputfiles = "se output files = *.root" if self._args.log_to_se else "se output files = *.log *.root",
				backend = backend,
				partitionlfnmodifier = "partition lfn modifier = " + self._args.partition_lfn_modifier if (self._args.partition_lfn_modifier != None) else ""
		)

		self.modify_replacing_dict()

		self.replaceLines(gcConfigFileContent, self.replacingDict)
		for index, line in enumerate(gcConfigFileContent):
			gcConfigFileContent[index] = line.replace("$CMSSW_BASE", os.environ.get("CMSSW_BASE", ""))
			gcConfigFileContent[index] = line.replace("$X509_USER_PROXY", os.environ.get("X509_USER_PROXY", ""))

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
		log.info("\nMerge outputs in one file per nick using")
		if not self.remote_se:
			log.info("artusMergeOutputs.py %s" % self.projectPath)
		log.info("artusMergeOutputsWithGC.py %s" % (self.localProjectPath if self.remote_se else self.projectPath))

		if exitCode != 0:
			log.error("Exit with code %s.\n\n" % exitCode)
			log.info("Dump configuration:\n")
			log.info(self._configFilename)

		return exitCode

	def modify_replacing_dict(self):
		pass # to be overwritten by deriving classes


	def measurePerformance(self, profTool):
		"""run Artus with profiler"""
		
		profile_cpp.profile_cpp(
				command=self._executable+" "+self._configFilename,
				profiler=profTool,
				output_dir=os.path.dirname(self._args.output_file)
		)
		
		return 0


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
