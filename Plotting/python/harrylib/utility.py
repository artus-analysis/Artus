#!/usr/bin/python

from __future__ import print_function

#### Helper Classes
#######################

class Enum(object):
	"""Bitflag Enum class"""
	def __init__(self, *enumList, **setEnumList):
		self.__dict__ = dict( zip( enumList, [ 2**ind for ind in range(len(enumList)) ] ) , **setEnumList)
	def __setitem__(self, name, value):
		self.__dict__[name] = value
	def add(self, name):
		self.__dict__[name] = 2**(math.log(max(self.__dict__.values(),2)) + 1)

#### Verbosity
#######################

verbosityLevels = Enum("ALWAYS", "INFO", "PRGFLOW", "PRGACTION", "DETAIL", "MICRO")
verbosityPrefix = {
	verbosityLevels.ALWAYS    : "     ",
	verbosityLevels.INFO      : "#    ",
	verbosityLevels.PRGFLOW   : "o    ",
	verbosityLevels.PRGACTION : "-o   ",
	verbosityLevels.LVL3      : "--o  ",
	verbosityLevels.LVL4      : "---o ",
	}

def vprint(message, verbosityLevel = verbosityLevels.ALWAYS, minLevel = True):
	if outVerbosity() == verbosityLevel or ( minLevel and outVerbosity() > verbosityLevel ):
		print("[%s] %s %s" % (time.asctime(), verbosityPrefix[verbosityLevel], message))

def eprint(message, verbosityLevel = verbosityLevels.ALWAYS, minLevel = True):
	if errVerbosity() == verbosityLevel or ( minLevel and errVerbosity() > verbosityLevel ):
		print("<%s> %s %s" % (time.asctime(), verbosityPrefix[verbosityLevel], message), file = sys.stderr)

def globalSetupProxy(fun, default, new = None):
	"Set or return a globally defined value"
	if new != None:
		fun.setting = new
	try:
		return fun.setting
	except:
		return default

def outVerbosity(setTo = None):
	"""
	return current STDOUT verbosity
	
	optional:
	    setTo VERBLVL    set STDOUT verbosity to VERBLVL
	"""
	return globalSetupProxy(outVerbosity, verbosityLevels.ALWAYS, setTo)
def errVerbosity(setTo = None):
	"""
	return current STDERR verbosity
	
	optional:
	    setTo VERBLVL    set STDERR verbosity to VERBLVL
	"""
	return globalSetupProxy(errVerbosity, verbosityLevels.ALWAYS, setTo)

