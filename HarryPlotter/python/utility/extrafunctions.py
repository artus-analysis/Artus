# -*- coding: utf-8 -*-

"""
Collection of functions to make plotting life faster
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import os
import subprocess
from difflib import SequenceMatcher

import ROOT


def show_plot(filename, viewer):
	log.info("Opening Plot " + filename + " with " + viewer)
	FNULL = open(os.devnull, 'w')
	subprocess.Popen([viewer, filename], stdout=FNULL, stderr=FNULL)

def show_plot_userpc(filename, viewer, user, userpc):
	"""Open the plot, but on the users desktop machine."""
	subprocess.call(['rsync', filename,
						  '%s:/usr/users/%s/plot.pdf' % (userpc, user)])

	# check if the imageviewer is running on the users local machine:
	p = subprocess.Popen(['ssh', userpc, 'ps', 'aux', '|', 'grep',
					'"%s /usr/users/%s/plot.%s"' % (viewer,
					user, filename.split(".")[-1]), '|', 'grep',
					 '-v', 'grep', '|', 'wc', '-l'], stdout=subprocess.PIPE)
	out, err = p.communicate()

	# if its not already running, start!
	if out[:1] == '0':
		print "\nOpening %s..." % viewer
		subprocess.Popen(['ssh', userpc,
		'DISPLAY=:0 %s /usr/users/%s/plot.%s &' % (viewer, user, filename.split(".")[-1])])

def isfloat(element):
	try:
		float(element)
		return True
	except ValueError:
		return False

def merge_sequences(*sequences):
	"""
	Merge lists preserving relative order

	:param sequences: lists to merge
	:type seuqnces: list[list]
	:returns: ordered union of all lists
	:rtype: list

	:see: `Stackoverflow thread<http://stackoverflow.com/questions/14241320/interleave-different-length-lists-elimating-duplicates-and-preserve-order-in-py>`_
	:raises ValueError: If individual sequences have non-unique elements or several
	                    sequences have the same elements in different order
	"""
	if len(sequences) == 1:
		return sequences[0]
	if len(sequences) == 2:
		return _merge_sequences(*sequences)
	if len(sequences) % 2 == 0:
		return _merge_sequences(*[_merge_sequences(sequences[idx-1], sequences[idx]) for idx in xrange(1,len(sequences),2)])
	return _merge_sequences(*[_merge_sequences(sequences[idx-1], sequences[idx]) for idx in xrange(1,len(sequences),2)] + [sequences[-1]])

def _merge_sequences(seq1,seq2):
	"""http://stackoverflow.com/questions/14241320/interleave-different-length-lists-elimating-duplicates-and-preserve-order-in-py"""
	sm=SequenceMatcher(a=seq1,b=seq2)
	res = []
	for (op, start1, end1, start2, end2) in sm.get_opcodes():
		if op == 'equal' or op=='delete':
			#This range appears in both sequences, or only in the first one.
			res += seq1[start1:end1]
		elif op == 'insert':
			#This range appears in only the second sequence.
			res += seq2[start2:end2]
		elif op == 'replace':
			#There are different ranges in each sequence - add both.
			res += seq1[start1:end1]
			res += seq2[start2:end2]
	return res
