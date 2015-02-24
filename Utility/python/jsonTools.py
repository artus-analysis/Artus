
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import collections
import copy
import json
import os
import pprint
import sys

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

import Artus.Utility.tools as tools


class JsonDict(dict):
	"""
	Class that stores python dicts and offers some additional JSON functionality
	"""
	
	# static members as global settings
	PATH_TO_ROOT_CONFIG = "config"
	ALWAYS_DO_COMMENTS = True
	COMMENT_DELIMITER = "#"

	def __init__(self, jsonDict={}):
		"""
		Constructor
		The parameter jsonDict can be of typ dict, string or list
		If jsonDict is of type string, it is first assumed to be a filename (can be JSON
		text file or ROOT file (with TObjString object named PATH_TO_ROOT_CONFIG in it).
		Otherwise, the string is tried to be evaluated as python code.
		If jsonDict is of type list, the list is converted into a JsonDictList
		and all items are merged into one single JsonDict
		"""
	
		if isinstance(jsonDict, dict):
			dict.__init__(self, jsonDict)
		elif isinstance(jsonDict, basestring):
			if os.path.exists(os.path.expandvars(jsonDict)):
				dict.__init__(self, JsonDict.readJsonDict(jsonDict))
			else:
				dict.__init__(self, json.loads(jsonDict.replace("'", "\"")))
		elif isinstance(jsonDict, collections.Iterable):
			dict.__init__(self, JsonDict.mergeAll(*jsonDict))
		else:
			raise TypeError("Unsupported type \"%s\" in JsonDict constructor", type(jsonDict))
		if JsonDict.ALWAYS_DO_COMMENTS:
			JsonDict.deepuncomment(self)
	
	def __add__(self, jsonDict2):
		""" merges two JSON dicts and returns a new object """
		
		jsonDict1 = JsonDict(self)
		JsonDict.deepmerge(jsonDict1, jsonDict2)
		return jsonDict1
	
	def merge(self, jsonDict2):
		""" merges two JSON dicts and returns a new object """
		return (self + jsonDict2)
	
	@staticmethod
	def mergeAll(*jsonDicts):
		""" merge all given parameters into one JSON dict """
		if len(jsonDicts) == 1:
			return JsonDict(jsonDicts[0])
		else:
			return reduce(lambda jsonDict1, jsonDict2: JsonDict(jsonDict1) + JsonDict(jsonDict2), jsonDicts)
	
	def __sub__(self, jsonDict2):
		""" compares two JSON dicts and returns two diff dicts """
		return JsonDict.deepdiff(self, jsonDict2)
	
	def diff(self, jsonDict2):
		""" compares two JSON dicts and returns two diff dicts """
		return (self - jsonDict2)
	
	def __mul__(self, jsonDict2):
		"""
		expands all possible combinations of second-layer dictionaries
		returns new JsonDict { keyA1_keyB1 : valueA1+valueB1, ... }
		"""
		
		jsonDict = JsonDict()
		for key1, value1 in self.items():
			for key2, value2 in jsonDict2.items():
				#key = (str(key1) if len(self) > 1 else "") + \
				#      ("_" if len(self) > 1 and len(jsonDict2) > 1 else "") + \
				#      (str(key2) if len(jsonDict2) > 1 else "")
				key = str(key1) + "_" + str(key2)
				jsonDict[key] = JsonDict(value1) + JsonDict(value2)
		return jsonDict
	
	def expand(self, jsonDict2):
		"""
		expands all possible combinations of second-layer dictionaries
		returns new JsonDict { keyA1_keyB1 : valueA1+valueB1, ... }
		"""
		return (self * jsonDict2)
	
	@staticmethod
	def expandAll(*jsonDicts):
		""" expand all given parameters into one JSON dict """
		if len(jsonDicts) == 1:
			return JsonDict(jsonDicts[0])
		else:
			return reduce(lambda jsonDict1, jsonDict2: JsonDict(jsonDict1) * JsonDict(jsonDict2), jsonDicts)
	
	def doIncludes(self):
		""" resolves the includes and returns a new object """
		return JsonDict(JsonDict.deepinclude(self))
	
	def doNicks(self, nick="default"):
		""" resolves the nicks and returns a new object """
		return JsonDict(JsonDict.deepresolvenicks(self, nick))
	
	def doComments(self):
		""" resolves the comments and returns a new object """
		return JsonDict.deepuncomment(JsonDict(copy.deepcopy(self)))
	
	def doExpandvars(self):
		""" expands environment variables in dictionary values """
		return JsonDict(JsonDict.deepexpandvars(self))
	
	def __str__(self):
		""" converts JSON dict to a string """
		return self.toString()
	
	def toString(self, indent=4, sort_keys=True, **kwargs):
		""" converts JSON dict to a string """
		return json.dumps(self, indent=indent, sort_keys=sort_keys, **kwargs)

	def save(self, fileName, indent=None, sort_keys=True, **kwargs):
		""" write JSON dict to a file """
		with open(fileName, "w") as jsonFile: json.dump(self, jsonFile, indent=indent, sort_keys=sort_keys, **kwargs)

	@staticmethod
	def readJsonDict(fileName):
		"""
		reads JSON dictionary from file
		fileName can point to a JSON text file or to a ROOT file containing
		a TObjString object named as specified by PATH_TO_ROOT_CONFIG
		"""
		
		fileName = os.path.expandvars(fileName)
		if not os.path.exists(fileName):
			log.critical("File \"%s\" does not exist!" % fileName)
			sys.exit(1)

		jsonStrings = []
		# read all cycles of JsonDict.PATH_TO_ROOT_CONFIG from ROOT file and merge them
		if os.path.splitext(fileName)[1] == ".root":
			rootFile = ROOT.TFile(fileName, "READ")
			listOfKeys = rootFile.GetListOfKeys()
			for keyIndex in range(listOfKeys.GetSize()):
				namecycle = "%s;%d" % (listOfKeys.At(keyIndex).GetName(), listOfKeys.At(keyIndex).GetCycle())
				if namecycle.startswith(JsonDict.PATH_TO_ROOT_CONFIG):
					jsonStrings.append(str(rootFile.Get(namecycle).GetString()))
			rootFile.Close()
			if len(jsonStrings) == 0:
				log.critical("Could not read \"%s\" from file \"%s\"!" % (JsonDict.PATH_TO_ROOT_CONFIG, rootFileName))
				sys.exit(1)
		else:
			with open(fileName, "r") as jsonFile:
				jsonStrings.append(jsonFile.read())
	
		jsonDict = JsonDict()
		try:
			for jsonString in [jsonStrings[-1]]: #jsonStrings[::-1]: # TODO need cmore careful merging of lists
				jsonDict += JsonDict(json.loads(jsonString))
		except ValueError, e:
			log.critical("Invalid JSON syntax in file \"%s\"." % fileName)
			log.critical(str(e))
			sys.exit(1)
		return jsonDict

	@staticmethod
	def deepmerge(dictWithHigherPriority, dictWithLowerPriority):
		"""
		deepmerge of two dictionaries.
		Entries from dictWithLowerPriority are recursively merged into
		dictWithHigherPriority in case, nothing has to be overwritten.
		"""
		
		for key, value in dictWithLowerPriority.items():
			if isinstance(value, dict):
				newTarget = dictWithHigherPriority.setdefault(key, {})
				if newTarget == None: dictWithHigherPriority.pop(key)
				else: JsonDict.deepmerge(newTarget, value)
			elif isinstance(value, collections.Iterable) and not isinstance(value, basestring):
				dictWithHigherPriority.setdefault(key, []).extend(value)
			elif dictWithHigherPriority != None and key not in dictWithHigherPriority:
				dictWithHigherPriority[key] = value
		return dictWithHigherPriority

	@staticmethod
	def deepdiff(dictA, dictB):
		"""
		deepdiff of two dictionaries.
		returns one dictionary for each given dictionary, that contains differences to other dictionary
		"""
		
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

	@staticmethod
	def deepinclude(jsonDict):
		"""
		resolves/replaces include options in JSON dictionaries
		protected property names are "include" which is followed by a list of files to include
		and "property" which is used as a property name in the included file to include just one property
		"""
		
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
						elif isinstance(tmpResult, collections.Iterable) and not isinstance(tmpResult, basestring):
							if result == JsonDict():
								result = []
							result.extend(tmpResult)
						else:
							result = tmpResult
				elif key == "property":
					result = JsonDict.deepinclude(value)
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

	@staticmethod
	def deepresolvenicks(jsonDict, nick="default"):
		"""
		resolves/replaces nick options in JSON dictionaries
		"nick" is a protected property name which is followed by a dict with nick name regexs as keys
		if no matching nick name is found, it is looked for "default".
		"""
		
		result = None
		if isinstance(jsonDict, dict):
			result = JsonDict()
			for key, value in jsonDict.items():
				if isinstance(value, dict) and isinstance(value.get("nick"), dict):
					nickDict = value["nick"]
					tmpValue = nickDict.get(tools.matchingItem(nickDict.keys(), nick),
					                        nickDict.get("default"))
					if tmpValue != None:
						result[key] = JsonDict.deepresolvenicks(tmpValue, nick=nick)
				else:
					result[key] = JsonDict.deepresolvenicks(value, nick=nick)
		else:
			result = copy.deepcopy(jsonDict)
		return result
	
	@staticmethod
	def deepuncomment(jsonDict):
		"""
		remove all comments from this JSON dictionary
		comments are keys, values and list entries starting with COMMENT_DELIMITER
		editing in place.
		"""
		if isinstance(jsonDict, dict):
			for key, value in jsonDict.items():
				if key.strip().startswith(JsonDict.COMMENT_DELIMITER):
					pass
					#del jsonDict[key]
				else:
					if isinstance(value, basestring) and value.strip().startswith(JsonDict.COMMENT_DELIMITER):
						del jsonDict[key]
					elif isinstance(value, collections.Iterable) and not isinstance(value, basestring):
						for index, element in enumerate(value):
							if isinstance(element, basestring) and element.strip().startswith(JsonDict.COMMENT_DELIMITER):
								del value[index]
					if isinstance(value, dict):
						JsonDict.deepuncomment(value)
		return jsonDict

	@staticmethod
	def deepexpandvars(jsonDict):
		"""
		call os.path.expandvars recursively for each string-type value in this dictionary
		"""
		
		result = None
		if isinstance(jsonDict, dict):
			result = JsonDict()
			for key, value in jsonDict.items():
				result[key] = JsonDict.deepexpandvars(value)
		elif isinstance(jsonDict, collections.Iterable) and not isinstance(jsonDict, basestring):
			result = []
			for item in jsonDict:
				result.append(JsonDict.deepexpandvars(item))
		elif isinstance(jsonDict, basestring):
			result = os.path.expandvars(jsonDict)
		else:
			result = jsonDict
		return result

def print_comments_from_json_files(json_dir, comment_key):
	"""print comments from all json files in a directory"""
	#TODO generalize this. works with any string, not only comments

	files = [os.path.join(json_dir, f) for f in os.listdir(json_dir) if f.endswith(".json")]

	log.info(tools.get_colored_string("json files:", 'cyan'))
	for f in files:
		d = JsonDict(f).doIncludes().doComments()
		string = "\t" + tools.get_colored_string(os.path.basename(f), "yellow")
		if comment_key in d:
			string += ("\n\t\t" + d[comment_key])
		log.info(string)
