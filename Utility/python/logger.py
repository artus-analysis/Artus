
# -*- coding: utf-8 -*-

import argparse
import logging
import os
import subprocess
import sys
import threading
'''
Global logging module.

loggingParser
	argparse Parser for inclusion to main Parser as parent.

print
	redirector of print through the logging module.
	/!\ this is a function and requires the import of print_function from __future__
'''

log = logging.getLogger(__name__)

# add-on to be included in a main argument parser
loggingParser = argparse.ArgumentParser(add_help=False)

_loggingOptionsGroup = loggingParser.add_argument_group("Logging options")
_loggingOptionsGroup.add_argument("--log-level",
                                default = "debug",
                                choices = ["debug", "info", "warning", "error", "critical"],
                                help    = "Detail level of logging. [Default: %(default)s]")
_loggingOptionsGroup.add_argument("--log-files",
                               nargs    = '*',
                               help     = "List of files to log to. [Default: %(default)s]")
_loggingOptionsGroup.add_argument("--log-std",
                                default = "NONE",
                                choices = ["stdout", "stderr", "NONE"],
                                help    = "List of streams to log to. [Default: %(default)s]")

def initLogger(argParserArgs=None, name="", logLevel="debug", logFiles=[], logStreams = []):
	'''
	Initialize a logger for usage.

	Optional:
	  argParserArgs
	                argparse namespace instance compatible with this module's parser
	  name
	                The name of the logger to initalize
	  logLevel
	                minimum logging level required for messages to be logged
	  logFiles
	                list of names of files to direct log output to
	  logStreams [stderr, stdout]
	                list of names of streams to direct log output to

	Returns:
	  _thisLogger
	                initialized logger instance
	'''
	_thisLogger = logging.getLogger(name)
	try:
		logLevel = argParserArgs.log_level
	except AttributeError:
		pass
	try:
		_thisLogger.setLevel(int(logLevel))
	except ValueError:
		_thisLogger.setLevel(getattr(logging, logLevel.upper()))
	### TODO: read LDFormat dynamically by name from initLogger attributes
	levelDependentFormatter = LevelDependentFormatter(initLogger.LDFormatDefault)
	try:
		logStreams.extend(argParserArgs.log_std)
	except AttributeError:
		pass
	try:
		logFiles.extend(argParserArgs.log_files)
	except AttributeError:
		pass
	loggingHandlers = []
	for logStream in logStreams:
		if logStream in 'sys.stdout':
			loggingHandlers.append(logging.StreamHandler(sys.stdout))
		elif logStream in 'sys.stderr':
			loggingHandlers.append(logging.StreamHandler(sys.stderr))
		elif logStream in 'NONE':
			continue
		else:
			raise ValueError('Invalid stream designator for logging: %s' % logStream)
	for logFile in logFiles:
		loggingHandlers.append(logging.FileHandler(logFile))
	for loggingHandler in loggingHandlers:
		loggingHandler.setFormatter(levelDependentFormatter)
		_thisLogger.addHandler(loggingHandler)
	return _thisLogger
# By-name-reference default dictionaries
initLogger.LDFormatDefault = {
		logging.DEBUG   : "%(levelname)s (%(filename)s.%(lineno)d): %(message)s",
		logging.INFO	: "%(message)s",
		logging.WARNING : "%(levelname)s: %(message)s"
	}

# Custom formatter with level-dependent formats
class LevelDependentFormatter(logging.Formatter):

	def __init__(self, fmtDict):
		defaultDebugFmt = "%(levelname)s (%(filename)s.%(lineno)d): %(message)s"
		noFmt = "%(message)s"
		logging.Formatter.__init__(self, fmt=defaultDebugFmt)
		fmtDict[0] = defaultDebugFmt
		fmtDict[100] = noFmt
		self._fmtItems = sorted(fmtDict.items(), key=lambda item: item[0])

	# format message according to format rule for this or next smaller level in fmtDict
	def format(self, record):
		for levelno, fmt in self._fmtItems:
			if record.levelno >= levelno:
				self._fmt = fmt
		return logging.Formatter.format(self, record)


# Pipe class that can be used to redirect output of subprocess to a logger
class LogPipe(threading.Thread):

	# setup the object with a logger and a loglevel and start the thread
	def __init__(self, level=100):
		threading.Thread.__init__(self)
		self.daemon = False
		self.level = level
		self.fdRead, self.fdWrite = os.pipe()
		self.pipeReader = os.fdopen(self.fdRead)
		self.start()

	# return the write file descriptor of the pipe
	def fileno(self):
		return self.fdWrite

	# run the thread, logging everything
	def run(self):
		for line in iter(self.pipeReader.readline, ''):
			log.log(self.level, line.strip('\n'))
		self.pipeReader.close()

	# close the write end of the pipe.
	def close(self):
		os.close(self.fdWrite)


# wrapper for subprocess.call with connects with the logger and outputs everything
def subprocessCall(args, **kwargs):
	logPipe = None
	if len(logging.root.handlers) > 1:
		logPipe = LogPipe(100)
		kwargs["stdout"] = logPipe
		kwargs["stderr"] = logPipe
	
	exitCode = subprocess.call(args, **kwargs)

	if logPipe:
		logPipe.close()

	return exitCode
	
