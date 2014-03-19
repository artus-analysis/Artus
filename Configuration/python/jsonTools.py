
# -*- coding: utf-8 -*-

import collections
import copy
import json
import logging
import os
import pprint
import ROOT
import sys


# Class that stores python dicts and offers some additional JSON functionality
class JsonDict(dict):
	
	# static members as global settings
	PATH_TO_ROOT_CONFIG = "config"
	ALWAYS_DO_COMMENTS = True

	# Constructor
	# The parameter jsonDict can be of typ dict, string or list
	# If jsonDict is of type string, it is first assumed to be a filename (can be JSON
	# text file or ROOT file (with TObjString object named PATH_TO_ROOT_CONFIG in it).
	# Otherwise, the string is tried to be evaluated as python code.
	# If jsonDict is of type list, the list is converted into a JsonDictList
	# and all items are merged into one single JsonDict
	def __init__(self, jsonDict={}):
		if isinstance(jsonDict, dict):
			dict.__init__(self, jsonDict)
		elif isinstance(jsonDict, basestring):
			if os.path.exists(os.path.expandvars(jsonDict)):
				dict.__init__(self, JsonDict.readJsonDict(jsonDict))
			else:
				dict.__init__(self, json.loads(jsonDict))
		elif isinstance(jsonDict, collections.Iterable):
			dict.__init__(self, JsonDict.mergeAll(*jsonDict))
		else:
			raise TypeError("Unsupported type \"%s\" in JsonDict constructor", type(jsonDict))
		if JsonDict.ALWAYS_DO_COMMENTS:
			JsonDict.deepuncomment(self)
	
	# merges two JSON dicts and returns a new object
	def __add__(self, jsonDict2):
		jsonDict1 = JsonDict(self)
		JsonDict.deepmerge(jsonDict1, jsonDict2)
		return jsonDict1
	
	# merges two JSON dicts and returns a new object
	def merge(self, jsonDict2):
		return (self + jsonDict2)
	
	@staticmethod
	def mergeAll(*jsonDicts):
		if len(jsonDicts) == 1:
			return JsonDict(jsonDicts[0])
		else:
			return reduce(lambda jsonDict1, jsonDict2: JsonDict(jsonDict1) + JsonDict(jsonDict2), jsonDicts)
	
	# compares two JSON dicts and returns two diff dicts
	def __sub__(self, jsonDict2):
		return JsonDict.deepdiff(self, jsonDict2)
	
	# compares two JSON dicts and returns two diff dicts
	def diff(self, jsonDict2):
		return (self - jsonDict2)
	
	# expands all possible combinations of second-layer dictionaries
	# returns new JsonDict { keyA1_keyB1 : valueA1+valueB1, ... }
	def __mul__(self, jsonDict2):
		jsonDict = JsonDict()
		for key1, value1 in self.items():
			for key2, value2 in jsonDict2.items():
				key = (str(key1) if len(self) > 1 else "") + \
				      ("_" if len(self) > 1 and len(jsonDict2) > 1 else "") + \
				      (str(key2) if len(jsonDict2) > 1 else "")
				jsonDict[key] = JsonDict(value1) + JsonDict(value2)
		return jsonDict
	
	# expands all possible combinations of second-layer dictionaries
	# returns new JsonDict { keyA1_keyB1 : valueA1+valueB1, ... }
	def expand(self, jsonDict2):
		return (self * jsonDict2)
	
	@staticmethod
	def expandAll(*jsonDicts):
		if len(jsonDicts) == 1:
			return JsonDict(jsonDicts[0])
		else:
			return reduce(lambda jsonDict1, jsonDict2: JsonDict(jsonDict1) * JsonDict(jsonDict2), jsonDicts)
	
	# resolves the includes and returns a new object
	def doIncludes(self):
		return JsonDict(JsonDict.deepinclude(self))
	
	# resolves the comments and returns a new object
	def doComments(self):
		return JsonDict.deepuncomment(JsonDict(copy.deepcopy(self)))
	
	# converts JSON dict to a string
	def __str__(self):
		return self.toString()
	
	# converts JSON dict to a string
	def toString(self, indent=4, sort_keys=True, **kwargs):
		return json.dumps(self, indent=indent, sort_keys=sort_keys, **kwargs)

	# write JSON dict to a file
	def save(self, fileName, indent=None, sort_keys=True, **kwargs):
		with open(fileName, "w") as jsonFile: json.dump(self, jsonFile, indent=indent, sort_keys=sort_keys, **kwargs)

	# reads JSON dictionary from file
	# fileName can point to a JSON text file or to a ROOT file containing
	# a TObjString object named as specified by PATH_TO_ROOT_CONFIG
	@staticmethod
	def readJsonDict(fileName):
		fileName = os.path.expandvars(fileName)
		if not os.path.exists(fileName):
			logging.getLogger(__name__).critical("File \"%s\" does not exist!" % fileName)
			sys.exit(1)

		jsonString = ""
		if os.path.splitext(fileName)[1] == ".root":
			rootFile = ROOT.TFile(fileName, "READ")
			jsonString = rootFile.Get(JsonDict.PATH_TO_ROOT_CONFIG)
			if not jsonString:
				logging.getLogger(__name__).critical("Could not read \"%s\" from file \"%s\"!" % (JsonDict.PATH_TO_ROOT_CONFIG, rootFileName))
				sys.exit(1)
			jsonString = str(jsonString.GetString())
			rootFile.Close()
		else:
			with open(fileName, "r") as jsonFile:
				jsonString = jsonFile.read()
	
		jsonDict = {}
		try:
			jsonDict = json.loads(jsonString)
		except ValueError, e:
			logging.getLogger(__name__).critical("Invalid JSON syntax in file \"%s\"." % fileName)
			logging.getLogger(__name__).critical(str(e))
			sys.exit(1)
		return JsonDict(jsonDict)

	# deepmerge of two dictionaries.
	# Entries from dictWithLowerPriority are recursively merged into
	# dictWithHigherPriority in case, nothing has to be overwritten.
	@staticmethod
	def deepmerge(dictWithHigherPriority, dictWithLowerPriority):
		for key, value in dictWithLowerPriority.items():
			if isinstance(value, dict):
				newTarget = dictWithHigherPriority.setdefault(key, {})
				if newTarget == None: dictWithHigherPriority.pop(key)
				else: JsonDict.deepmerge(newTarget, value)
			elif dictWithHigherPriority != None and key not in dictWithHigherPriority:
				dictWithHigherPriority[key] = value
		return dictWithHigherPriority

	# deepdiff of two dictionaries.
	# returns one dictionary for each given dictionary, that contains differences to other dictionary
	@staticmethod
	def deepdiff(dictA, dictB):
		diffDictA = {}
		diffDictB = {}
		for key in set(dictA.keys() + dictB.keys()):
			if key not in dictA:
				diffDictB[key] = dictB[key]
			elif key not in dictB:
				diffDictA[key] = dictA[key]
			elif dictA[key] != dictB[key]:
				if isinstance(dictA[key], dict) and isinstance(dictB[key], dict):
					diffDictA2, diffDictB2 = JsonDict.deepdiff(dictA[key], dictB[key])
					diffDictA[key] = diffDictA2
					diffDictB[key] = diffDictB2
				else:
					diffDictA[key] = dictA[key]
					diffDictB[key] = dictB[key]
		return diffDictA, diffDictB

	# resolves/replaces include options in JSON dictionaries
	# protected property names are "include" which is followed by a list of files to include
	# and "property" which is used as a property name in the included file to include just one property
	@staticmethod
	def deepinclude(jsonDict):
		result = None
		if isinstance(jsonDict, dict):
			result = JsonDict()
			for key, value in jsonDict.items():
				if key == "include":
					if isinstance(value, basestring):
						value = [value]
					for includeFile in value:
						tmpResult = JsonDict.deepinclude(JsonDict(includeFile))
						if isinstance(tmpResult, dict):
							result += JsonDict(tmpResult)
						else:
							result = tmpResult
				elif key == "property":
					result = copy.deepcopy(value)
				else:
					result[key] = JsonDict.deepinclude(value)
		elif isinstance(jsonDict, collections.Iterable) and not isinstance(jsonDict, basestring):
			result = []
			for element in jsonDict:
				tmpResult = JsonDict.deepinclude(element)
				if isinstance(tmpResult, collections.Iterable) and not isinstance(tmpResult, basestring):
					result.extend(tmpResult)
				else:
					result.append(tmpResult)
		else:
			result = copy.deepcopy(jsonDict)
		return result

	# remove all comments from this JSON dictionary
	# comments are keys, values and list entries starting with #
	# editing in place
	@staticmethod
	def deepuncomment(jsonDict):
		if isinstance(jsonDict, dict):
			for key, value in jsonDict.items():
				if key.strip().startswith("#"):
					del jsonDict[key]
				else:
					if isinstance(value, basestring) and value.strip().startswith("#"):
						del jsonDict[key]
					elif isinstance(value, collections.Iterable) and not isinstance(value, basestring):
						for index, element in enumerate(value):
							if isinstance(element, basestring) and element.strip().startswith("#"):
								del value[index]
					if isinstance(value, dict):
						JsonDict.deepuncomment(value)
		return jsonDict

