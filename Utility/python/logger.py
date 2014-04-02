# -*- coding: utf-8 -*-

from __future__ import print_function
import argparse
import logging
import os
import subprocess
import sys
import threading
'''
Global logging module.

mainLog
  The root python logging logger instance, suitable for general-purpose logging

loggingParser
  Default argparse Parser for inclusion to main Parser as parent.

getLoggingParser
  Return argparse Parser created based on current setup.

configureModuleGlobals
  Change the defaults for any non-local functions in the module.

vprint
  Perform print through the logging module to stdout.
  This is an alias for print not requiring the import of print_function.

eprint
  Perform print through the logging module to stderr.

print
  Alias for vprint.
  /!\ This is a function and requires 'from __future__ import print_function' in any module it is used!
'''

log = logging.getLogger(__name__)
mainLog = logging.getLogger()

def getLoggingParser(parserErrorLevels=[]):
	'''
	Return an argparse ArgumentParser implementing all settings of this module.
	'''
	_parserErrorLevels = parserErrorLevels or getLoggingParser.defaultErrorLevels
	_loggingParser = argparse.ArgumentParser(add_help=False)
	_verbosityOptionsGroup = _loggingParser.add_argument_group("Verbosity options")
	_verbosityOptionsGroup.add_argument('--verbose', '-v',
	                                   action  = 'store_true',
	                                   help    = "Full detail level for run-time information. [Default: %(default)s]")
	_verbosityOptionsGroup.add_argument("--out-level",
	                                   default = "info",
	                                   choices = _parserErrorLevels,
	                                   help    = "Detail level of run-time standard messages. [Default: %(default)s]")
	_verbosityOptionsGroup.add_argument("--err-level",
	                                   default = "info",
	                                   choices = _parserErrorLevels,
	                                   help    = "Detail level of run-time error messages. [Default: %(default)s]")

	_loggingOptionsGroup = _loggingParser.add_argument_group("Logging options")
	_loggingOptionsGroup.add_argument("--log-level",
	                                 default = "info",
	                                 choices = _parserErrorLevels,
	                                 help    = "Detail level of logging. [Default: %(default)s]")
	_loggingOptionsGroup.add_argument("--log-files",
	                                 nargs    = '*',
	                                 default  = [],
	                                 help     = "List of files to log to. [Default: %(default)s]")
	_loggingOptionsGroup.add_argument("--log-stream",
	                                 default = "NONE",
	                                 choices = ["stdout", "stderr", "NONE"],
	                                 help    = "Stream to log to. [Default: %(default)s or stdout if no file given]")
	return _loggingParser
getLoggingParser.defaultErrorLevels = ["notset","debug", "info", "warning", "error", "critical"]

# default Parser
loggingParser = getLoggingParser()

def configurePrints(argParserArgs=None, **kwargs):
	"""
	Set the features of the exported print functions.

	argParserArgs
	                argparse namespace instance compatible with this module's parser
	**kwargs
	                Defaults using Namespace notation
	"""
	try:
		kwargs.update(vars(argParserArgs))
	except TypeError:
		pass
	# re-create the printer logging imprints
	outLvl = ( kwargs.get('verbose',False) and 1 ) or getattr(logging, kwargs.get('out_level','info').upper())
	errLvl = ( kwargs.get('verbose',False) and 1 ) or getattr(logging, kwargs.get('err_level','info').upper())
	logInit(level = outLvl, Handler = logging.StreamHandler(stream=sys.stdout))(vprint)
	logInit(level = errLvl, Handler = logging.StreamHandler(stream=sys.stderr))(eprint)

def configureModule(argParserArgs=None, printKWArgs=None, logKWArgs=None):
	"""
	Call all configuration/initializations of this module.

	argParserArgs
	                argparse namespace instance compatible with this module's parser
	printKWArgs
	                Defaults for print initialization using Namespace notation
	logKWArgs
	                Defaults for logging initialization
	"""
	printKWArgs = printKWArgs or {}
	logKWArgs   = logKWArgs or {}
	configurePrints(argParserArgs=argParserArgs, **printKWArgs)
	_thislogger = initLogger(argParserArgs=argParserArgs, **logKWArgs)
	return _thislogger

def logInit(level = logging.DEBUG, name = None, formatter = None, formatStr = '%(message)s', dateFmt = None, Handler = None):
	"""
	Decorator for creating a logMsg method that writes directly to a logger.

	This decorator is primarily intended for creating print-like functions that wrap static, globally configured loggers.

	Optional:
	  level LVL
	                logging level of the underlying logger
	  name NAME
	                name/path of the underlying logger
	  formatter FMTR
	                formatter of the underlying logger
	  formatStr FSTR
	                format string to use for logging - ignored if FMTR is defined
	  dateFmt DFMT
	                date format to use for logging - ignored if FMTR is defined
	  Handler HNDL
	                Handler to use for logging
	"""
	def logInitDecorator(func):
		funcHandler = Handler or logging.StreamHandler(stream=sys.stdout)
		funcHandler.setFormatter((formatter or logging.Formatter(formatStr, dateFmt)))
		func.logger = logging.getLogger((name or func.__name__))
		func.logger.setLevel(level)
		func.logger.propagate = False
		func.logger.handlers = [funcHandler]
		def logMsg(messages, sep = ' ',msgLevel = 0):
			if hasattr(messages, '__iter__'):
				func.logger.log(msgLevel, sep.join(( str(msg) for msg in messages)))
			else:
				func.logger.log(msgLevel, messages)
		func.logMsg = logMsg
		return func
	return logInitDecorator
# default print_function'y
@logInit(Handler = logging.StreamHandler(stream=sys.stdout))
def vprint(msg, sep = ' ', level = logging.CRITICAL):
	"""
	Write to STDOUT logger

	msg
	                object or list of objects to be printed
	sep
	                separator string when printing multiple objects
	level
	                verbosity level of the message
	"""
	vprint.logMsg(msg, sep, level)
@logInit(Handler = logging.StreamHandler(stream=sys.stderr))
def eprint(msg, sep = ' ', level = logging.CRITICAL):
	"""
	Write to STDERR logger

	msg
	                object or list of objects to be printed
	sep
	                separator string when printing multiple objects
	level
	                verbosity level of the message
	"""
	eprint.logMsg(msg, sep, level)
# convenience aliases
print = vprint

def initLogger(argParserArgs=None, name="", logLevel="debug", logFiles=[], logStream='NONE', reinitialize=True):
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
	  logStreams [stderr, stdout, NONE]
	                list of names of streams to direct log output to
	  reinitialize
	                Overwrite any multi-option settings (instead of appending)

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
		logStream = argParserArgs.log_stream
	except AttributeError:
		pass
	try:
		logFiles.extend(argParserArgs.log_files)
	except AttributeError:
		pass
	loggingHandlers = []
	if logStream in ['stdout', 'sys.stdout']:
		loggingHandlers.append(logging.StreamHandler(sys.stdout))
	elif logStream in ['stderr','sys.stderr']:
		loggingHandlers.append(logging.StreamHandler(sys.stderr))
	elif logStream.upper() == 'NONE':
		pass
	else:
		raise ValueError('Invalid stream designator for logging: %s' % logStream)
	for logFile in logFiles:
		loggingHandlers.append(logging.FileHandler(logFile))
	# Fallback: logging enabled but no target, use STDOUT
	if not loggingHandlers:
		loggingHandlers.append(logging.StreamHandler(sys.stdout))
	if reinitialize:
		_thisLogger.handlers = []
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

