
# -*- coding: utf-8 -*-

import copy
import json
import logging
import os
import ROOT


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

# read JSON dictionary from root file or from JSON text file
def readJsonDict(fileName, pathInRootFile="config"):
	jsonDict = {}
	if os.path.splitext(fileName)[1] == ".root":
		rootFile = ROOT.TFile(fileName, "READ")
		jsonString = rootFile.Get(pathInRootFile)
		if not jsonString:
			logging.getLogger(__name__).critial("Could not read \"%s\" from file \"%s\"!" % (pathInRootFile, rootFileName))
			sys.exit(1)
		jsonString = str(jsonString.GetString())
		jsonDict = json.loads(jsonString)
		rootFile.Close()
	else:
		with open(fileName, "r") as jsonFile: jsonDict = json.load(jsonFile)
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

