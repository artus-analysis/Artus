
# -*- coding: utf-8 -*-

import collections
import copy
import json
import logging
import os
import ROOT
import sys


# deepmerge of two dictionaries.
# Entries from dictWithLowerPriority are recursively merged into
# dictWithHigherPriority in case, nothing has to be overwritten.
def deepmerge(dictWithHigherPriority, dictWithLowerPriority):
	for key, value in dictWithLowerPriority.items():
		if type(value) == dict:
			newTarget = dictWithHigherPriority.setdefault(key, {})
			if newTarget == None: dictWithHigherPriority.pop(key)
			else: deepmerge(newTarget, value)
		elif dictWithHigherPriority != None and key not in dictWithHigherPriority:
			dictWithHigherPriority[key] = value
	return dictWithHigherPriority

# deepdiff of two dictionaries.
# returns one dictionary for each given dictionary, that contains differences to other dictionary
def deepdiff(dictA, dictB):
	diffDictA = {}
	diffDictB = {}
	for key in set(dictA.keys() + dictB.keys()):
		if key not in dictA:
			diffDictB[key] = dictB[key]
		elif key not in dictB:
			diffDictA[key] = dictA[key]
		elif dictA[key] != dictB[key]:
			if type(dictA[key]) == dict and type(dictB[key]) == dict:
				diffDictA2, diffDictB2 = deepdiff(dictA[key], dictB[key])
				diffDictA[key] = diffDictA2
				diffDictB[key] = diffDictB2
			else:
				diffDictA[key] = dictA[key]
				diffDictB[key] = dictB[key]
	return diffDictA, diffDictB

# resolves/replaces include options in JSON dictionaries
# protected property names are "include" which is followed by a list of files to include
# and "property" which is used as a property name in the included file to include just one property
def deepinclude(jsonDict):
	result = None
	if type(jsonDict) == dict:
		result = {}
		for key, value in jsonDict.items():
			if key == "include":
				if isinstance(value, basestring):
					value = [value]
				for includeFile in value:
					tmpResult = deepinclude(readJsonDict(includeFile))
					if type(tmpResult) == dict:
						result = deepmerge(result, tmpResult)
					else:
						result = tmpResult
			elif key == "property":
				result = copy.deepcopy(value)
			else:
				result[key] = deepinclude(value)
	elif isinstance(jsonDict, collections.Iterable) and not isinstance(jsonDict, basestring):
		result = []
		for element in jsonDict:
			tmpResult = deepinclude(element)
			if isinstance(tmpResult, collections.Iterable) and not isinstance(tmpResult, basestring):
				result.extend(tmpResult)
			else:
				result.append(tmpResult)
	else:
		result = copy.deepcopy(jsonDict)
	return result

# remove all comments from JSON dictionary
# comments are keys, values and list entries starting with #
def deepuncomment(jsonDict):
	result = None
	if type(jsonDict) == dict:
		result = {}
		for key, value in jsonDict.items():
			if not key.strip().startswith("#"):
				if not isinstance(value, basestring) or not value.strip().startswith("#"):
					tmpValue = copy.deepcopy(value)
					if type(value) == dict:
						tmpValue = deepuncomment(value)
					elif isinstance(value, collections.Iterable) and not isinstance(value, basestring):
						tmpValue = []
						for element in value:
							if not isinstance(element, basestring) or not element.strip().startswith("#"):
								tmpValue.append(copy.deepcopy(element))
					result[key] = tmpValue
	return result

# read JSON dictionary from root file or from JSON text file
def readJsonDict(fileName, pathInRootFile="config"):
	fileName = os.path.expandvars(fileName)
	if not os.path.exists(fileName):
		logging.getLogger(__name__).critical("File \"%s\" does not exist!" % fileName)
		sys.exit(1)

	jsonString = ""
	if os.path.splitext(fileName)[1] == ".root":
		rootFile = ROOT.TFile(fileName, "READ")
		jsonString = rootFile.Get(pathInRootFile)
		if not jsonString:
			logging.getLogger(__name__).critical("Could not read \"%s\" from file \"%s\"!" % (pathInRootFile, rootFileName))
			sys.exit(1)
		jsonString = str(jsonString.GetString())
		rootFile.Close()
	else:
		with open(fileName, "r") as jsonFile:
			jsonString = jsonFile.read()
	
	jsonDict = {}
	print jsonString
	try:
		jsonDict = json.loads(jsonString)
	except ValueError, e:
		logging.getLogger(__name__).critical("Invalid JSON syntax in file \"%s\"." % fileName)
		logging.getLogger(__name__).critical(str(e))
		sys.exit(1)
	return jsonDict

# print JSON dictionary.
# If jsonDict is a path, the file will first be read in.
# If jsonDict is a string represenation of a dict, it will be evaluated
def printJsonDict(jsonDict, indent=4, sort_keys=True, **kwargs):
	if type(jsonDict) == str:
		jsonDict = jsonDict.strip()
		if jsonDict.startswith("{") and jsonDict.endswith("}"): jsonDict = eval(jsonDict)
		else: jsonDict = readJsonDict(jsonDict)
	logging.getLogger(__name__).info(json.dumps(jsonDict, indent=indent, sort_keys=sort_keys, **kwargs))

# merge multiple JSON dictionaries
# args can be of typ dict or string (path to JSON file or string represenation of a dict)
# returns the merged dictionary
def mergeJsonDicts(*args):
	mergedJsonDict = {}
	for arg in args:
		if type(arg) == dict: deepmerge(mergedJsonDict, arg)
		elif type(arg) == str: deepmerge(mergedJsonDict, readJsonDict(arg))
		else: logging.getLogger(__name__).error("Could not merge JSON dicts with object of type %s." % str(type(arg)))
	return mergedJsonDict

# write JSON dict to a file
def saveJsonDict(jsonDict, fileName):
	with open(fileName, "w") as jsonFile: json.dump(jsonDict, jsonFile, indent=None, sort_keys=True)

