
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array
import copy
import fcntl
import multiprocessing
import multiprocessing.pool
import os
import re
import sys
import termios
import textwrap
import shlex


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

def matching_sublist(input_list, whitelist=[], blacklist=[]):
	"""returns subset of input_list (non) matching the regexps."""
	whitelist_matches = []
	if len(whitelist) == 0:
		whitelist_matches = copy.deepcopy(input_list)
	else:
		tmp_input_list = copy.deepcopy(input_list)
		for regex in whitelist:
			indices_to_remove = []
			for index, item in enumerate(tmp_input_list):
				if not re.search(regex, item) is None:
					whitelist_matches.append(item)
					indices_to_remove.append(index)
			for index in indices_to_remove[::-1]:
				tmp_input_list.pop(index)
	
	output_list = []
	if len(blacklist) == 0:
		output_list = copy.deepcopy(whitelist_matches)
	else:
		for item in whitelist_matches:
			keep = True
			for regex in blacklist:
				if not item is None and not re.search(regex, item) is None:
					keep = False
					continue
			if keep:
				output_list.append(item)
	
	# warn if black/whitelisting changes the order of nicks
	indices = [input_list.index(out_nick) for out_nick in output_list]
	if not all(x<y for x, y in zip(indices, indices[1:])):
		log.warn("White-/blacklisting changed order of nicks!")
		original_order = [i for i in input_list if i in output_list]
		log.warn("Original order: "+", ".join(original_order))
		log.warn("New order     : "+", ".join(output_list))
	
	return output_list

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
	try:
		fcntl.ioctl(0, termios.TIOCGWINSZ, size, True)
		return (size[0], size[2])
	except:
		return size

def get_environment_variable(variable_name, fail_if_not_existing=True):
	"""get variable from os, throw error if not set"""
	try:
		value = os.environ[variable_name]
		return value
	except KeyError:
		if fail_if_not_existing:
			log.critical("'{}' not in environment variables. Maybe you forgot to source an ini file?".format(variable_name))
			sys.exit(1)
		return None

def get_colored_string(string, color='green'):
	d = {
		'red': '101',
		'green':'102',
		'yellow':'103',
		'blue':'104',
		'magenta':'104',
		'cyan':'106',
	}
	return "\033[0;{1};30m{0}\033[0m".format(string, d[color])

def get_indented_text(prefix, message, width=None):
	"""returns a text block which is line-wrapped and indented at a certain length."""
	if width is None:
		width = get_tty_size()[1]
	expanded_indent = textwrap.fill(prefix+'$', replace_whitespace=False)[:-1]
	subsequent_indent = ' ' * len(expanded_indent)

	# code below is needed to preserve line wrap:
	# http://stackoverflow.com/questions/1166317/python-textwrap-library-how-to-preserve-line-breaks
	tmp_wrapped_texts = []
	for line in message.splitlines():
		if line.strip() != '':
			tmp_wrapped_texts += textwrap.wrap(line,
				initial_indent=expanded_indent,
				width=width,
				subsequent_indent=subsequent_indent,
				break_long_words=False,
				replace_whitespace=False
			)
	return '\n'.join(['\n'.join(tmp_wrapped_texts)])

# sub-class of multiprocessing.Process that allows to switch off the deamon attribute
class NoDaemonProcess(multiprocessing.Process):
    def _get_daemon(self):
        return False
    def _set_daemon(self, value):
        pass
    daemon = property(_get_daemon, _set_daemon)
 
class NoDaemonProcessPool(multiprocessing.pool.Pool):
    Process = NoDaemonProcess

def parallelize(function, arguments_list, n_processes=1):
	if n_processes <= 1:
		results = []
		for arguments in arguments_list:
			results.append(function(arguments))
		return results
	else:
		pool = NoDaemonProcessPool(processes=max(1, min(n_processes, len(arguments_list))))
		results = pool.map_async(function, arguments_list)
		return results.get(9999999) # 9999999 is needed for KeyboardInterrupt to work: http://stackoverflow.com/questions/1408356/keyboard-interrupts-with-pythons-multiprocessing-pool


def hadd2(arguments):
	hadd(**arguments)

def hadd(target_file, source_files, hadd_args="", max_files=500):
	if len(source_files) == 0:
		log.critical("No source files specified to be merged!")
		sys.exit(1)
	source_files_chunks = [source_files[start_chunk_index:start_chunk_index+max_files] for start_chunk_index in xrange(0, len(source_files), max_files)]
	
	exit_code = 0
	for chunk_index, tmp_source_files in enumerate(source_files_chunks):
		tmp_target_file = "%s.hadd_%d.root" % (target_file, chunk_index)
		if chunk_index == len(source_files_chunks)-1:
			tmp_target_file = target_file
		
		last_target_file = ""
		if chunk_index > 0:
			last_target_file = "%s.hadd_%d.root" % (target_file, chunk_index-1)
		
		command = "hadd %s %s %s %s" % (hadd_args, tmp_target_file, " ".join(tmp_source_files), last_target_file)
		log.debug(command)
		exit_code = max(exit_code, logger.subprocessCall(shlex.split(command)))
		
		# remove last temp. merge result
		if len(last_target_file) > 0:
			os.remove(last_target_file)
	return exit_code

def write_dbsfile(input_files):
	dbsFileContent = ""
	for nickname, filelist in input_files.iteritems():
		dbsFileContent += "\n[" + nickname + "]\nnickname = " + nickname + "\n"
		for inputEntry in filelist:
			dbsFileContent += inputEntry + "\n"

	return dbsFileContent

def get_folder_size(folder):
	total_size = os.path.getsize(folder)
	for item in os.listdir(folder):
		itempath = os.path.join(folder, item)
		if os.path.isfile(itempath):
			total_size += os.path.getsize(itempath)
		elif os.path.isdir(itempath):
			total_size += get_folder_size(itempath)
	return total_size


def split_multiplication(formula):
	result = []
	index = 0
	for pos in range(index,len(formula)+1):
		if formula[index:pos].count('(') == formula[index:pos].count(')'):
			if pos == len(formula):
				if(len(result) > 0):
					result.append(split_multiplication(formula[index:pos]))
					return result
				else:
					if formula.startswith("("):
						return split_multiplication(formula[1:len(formula)-1])
					else:
						return formula
			else:
				if formula[pos] == '*':
					result.append(split_multiplication(formula[index:pos]))
					index = pos+1

def make_multiplication(splitted_formula):
	if isinstance(splitted_formula,basestring):
		return splitted_formula
	else:
		merged_formula = ''
		for item in splitted_formula:
			n_item = make_multiplication(item)
			if n_item != "1.0":
				merged_formula = ((merged_formula + "*(" + n_item + ")") if len(merged_formula)>0 else ("(" + n_item +")"))
		if len(merged_formula) > 0:
			return merged_formula
		else:
			return "(1.0)"

def clean_multiplication(formula):
	return make_multiplication(split_multiplication(formula))
