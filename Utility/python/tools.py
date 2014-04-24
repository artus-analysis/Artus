
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import re


def flattenList(listOfLists):
	"""
	flatten 2D list
	return [1, 2, 3, 4, ...] for input [[1, 2], [3, 4, ...], ...]
	"""
	return [item for subList in listOfLists for item in subList]

def matchingItem(stringList, regexForItem):
	"""
	find/return item in string list that matches the given regex
	returns None in case no matching item is found
	"""
	for item in stringList:
		if re.search(regexForItem, item) != None:
			return item
	return None

