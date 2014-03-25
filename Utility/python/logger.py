
# -*- coding: utf-8 -*-

import argparse
import logging
import os
import subprocess
import sys
import threading


log = logging.getLogger(__name__)


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


loggingParser = argparse.ArgumentParser(add_help=False)

logginOptionsGroup = loggingParser.add_argument_group("Logging options")
logginOptionsGroup.add_argument("--log-level", default="debug",
                                choices=["debug", "info", "warning", "error", "critical"],
                                help="Log level. [Default: debug]")
logginOptionsGroup.add_argument("--log-file", help="Log file. [Default: None (output on console)]")

def initLogger(argParserArgs=None, name=""):

	logLevel = "debug"
	logFile = None

	if argParserArgs:
		logLevel = argParserArgs.log_level
		logFile = argParserArgs.log_file

	logging.root.setLevel(getattr(logging, logLevel.upper()))
	
	# level dependent formats
	levelDependentFormatter = LevelDependentFormatter({
		logging.DEBUG   : "%(levelname)s (%(filename)s.%(lineno)d): %(message)s",
		logging.INFO	: "%(message)s",
		logging.WARNING : "%(levelname)s: %(message)s"
	})

	# add output to stdout by default, and file output if requested
	loggingHandlers = [logging.StreamHandler(sys.stdout)]
	if logFile:
		loggingHandlers.append(logging.FileHandler(logFile))
	
	for loggingHandler in loggingHandlers:
		loggingHandler.setFormatter(levelDependentFormatter)
		logging.root.addHandler(loggingHandler)
	
	log.info("Output will be logged to \"%s\"." % logFile)

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
	
