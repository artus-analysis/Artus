
# -*- coding: utf-8 -*-

import logging
import ArtusTools.Tools.logger as logger
log = logging.getLogger(__name__)

import array
import fcntl
import re
import termios


def flattenList(listOfLists):
	"""
	flatten 2D list
	return [1, 2, 3, 4, ...] for input [[1, 2], [3, 4, ...], ...]
	"""
	return [item for subList in listOfLists for item in subList]

def matchingItem(regexItems, string):
	"""
	find/return item in string list that matches the given regex
	returns None in case no matching item is found
	"""
	for item in regexItems:
		if re.search(item, string) != None:
			return item
	return None

# http://codereview.stackexchange.com/questions/21532/python-3-finding-common-patterns-in-pairs-of-strings
def longest_common_substring(S1, S2):
	M = [[0]*(1+len(S2)) for i in range(1+len(S1))]
	longest, x_longest = 0, 0
	for x in range(1,1+len(S1)):
		for y in range(1,1+len(S2)):
			if S1[x-1] == S2[y-1]:
				M[x][y] = M[x-1][y-1] + 1
				if M[x][y]>longest:
					longest = M[x][y]
					x_longest  = x
			else:
				M[x][y] = 0
	return S1[x_longest-longest: x_longest]

# http://codereview.stackexchange.com/questions/21532/python-3-finding-common-patterns-in-pairs-of-strings
def find_common_patterns(s1, s2): # used recursively
	if s1 == '' or s2 == '':
		return [], []
	com = longest_common_substring(s1, s2)
	if len(com) < 2:
		return ([(0, s1)], [(0, s2)])
	s1_bef, _, s1_aft = s1.partition(com)
	s2_bef, _, s2_aft = s2.partition(com)
	before = find_common_patterns(s1_bef, s2_bef)
	after = find_common_patterns(s1_aft, s2_aft)
	return (before[0] + [(1, com)] + after[0],
			before[1] + [(1, com)] + after[1])

def find_common_string(s1, s2):
	return "".join(pattern[1] if pattern[0] > 0 else "" for pattern in find_common_patterns(s1, s2)[0])

def get_tty_size():
	size = array.array("B", [0, 0, 0, 0])
	fcntl.ioctl(0, termios.TIOCGWINSZ, size, True)
	return (size[0], size[2])

