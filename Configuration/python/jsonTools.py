
# -*- coding: utf-8 -*-

import copy
import json
import logging

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

# merge multiple JSON dictionaries
# args can be of typ dict or string (path to JSON file)
# returns the merged dictionary
def mergeJsonDicts(*args):
	mergedJsonDict = {}
	for arg in args:
		if type(arg) == dict: deepmerge(mergedJsonDict, arg)
		elif type(arg) == str:
			with open(arg, "r") as jsonFile: deepmerge(mergedJsonDict, json.load(jsonFile))
		else: logging.getLogger(__name__).error("Could not merge JSON dicts with object of type %s." % str(type(arg)))
	return mergedJsonDict

# write JSON dict to a file
def saveJsonDict(jsonDict, fileName):
	with open(fileName, "w") as jsonFile: json.dump(jsonDict, jsonFile, indent=None, sort_keys=True)

