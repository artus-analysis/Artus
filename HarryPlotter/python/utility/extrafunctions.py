# -*- coding: utf-8 -*-

"""
Collection of functions to make plotting life faster
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import os
import subprocess

import ROOT


def show_plot(filename, viewer):
	log.info("Opening Plot " + filename + " with " + viewer)
	subprocess.Popen([viewer, filename])

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

