# -*- coding: utf-8 -*-
"""This module selects which plot function to use
- a predefined function from 
"""

import socket
import getpass
import os
import os.path
import sys
import copy
import matplotlib

from time import localtime, strftime, clock
from ROOT import gROOT, PyConfig
PyConfig.IgnoreCommandLineOptions = True  # prevents Root from reading argv
gROOT.SetBatch(True)  # no root blah blah

# basic functionality
import Artus.Plotting.tools.getroot as getroot  # read root files
import Artus.Plotting.tools.dictionaries as dictionaries # this plot dictionary should perhaps move?

# all plot modules
# use ls and imp to read all of them?
import Artus.Plotting.plot1d as plot1d
import Artus.Plotting.plot2d as plot2d
# import ... .Plotting.myhiggsplot as myhiggsplot # to come


def plot(op):
	"""Search for plots in the module and run them."""
	# dont display any graphics
	gROOT.SetBatch(True)
	startop = copy.deepcopy(op)
	whichfunctions = []
	plots = op.plots

	modules = [plot2d, plot1d]

	if op.verbose:
		showoptions(op)

	print "Number of files:", len(op.files)
	files = []
	for f in op.files:
		print "Using as file", 1 + op.files.index(f), ":", f
		files += [getroot.openfile(f, op.verbose)]

	if op.list:
		printfunctions(modules)
		sys.exit()

	if op.quantities:
		printquantities(files, op)
		sys.exit()

	remaining_plots = copy.deepcopy(plots)
	for module in modules:
		#print "Doing plots in", module.__name__, "..."
		if op.verbose:
			print "%1.2f | Start plotting" % clock()
		if not plots:
			print "Nothing to do. Please list the plots you want!"
			plots = []
		for p in plots:
			if hasattr(module, p):	# plot directly as a function
				print "Doing %s in %s" % (p, module.__name__)
				getattr(module, p)(files, op)
				remaining_plots.remove(p)
			elif module == plot1d and p in dictionaries.d_plots:	# if no function available, try dictionary
				print "New plot: (from dictionary)", p,
				plot1d.plotfromdict(files, op, p)
				remaining_plots.remove(p)
		if op != startop:
			whichfunctions += [p + " in " + module.__name__]
		if op.verbose:
			print "%1.2f | End" % clock()
	# remaining plots are given to the function_selector
	if len(remaining_plots) > 0:
		print "Doing remaining plots via function selector..."
		function_selector(remaining_plots, files, op)

	# check whether the options have changed and warn
	if op != startop:
		print "WARNING: The following options have been modified by a plot:"
		for key in dir(op):
			if "_" not in key and getattr(op, key) != getattr(startop, key):
				print " -", repr(key), "was:", getattr(startop, key), "is now:", getattr(op, key)
		print "These plots modify options:"
		for f in whichfunctions:
			print "  ", f
		print "This should not be the case! Options should not be changed within a plotting function."
		print "A solution could invoke copy.deepcopy() in such a case."


try:
	getobjectfromtree = profile(getobjectfromtree)
except NameError:
	pass  # not running with profiler


# function_selector: takes a list of plots and assigns them to the according funtion
def function_selector(plots, datamc, opt):
	for plot in plots:
		if '2D' in plot:
			plot2d.twoD(plot[3:], datamc, opt)
		elif "_all" in plot:
			plot1d.datamc_all(plot[:-4], datamc, opt)

		# for responseratio-plots
		elif 'responseratio' in plot and len(plot.split('_')) > 2:
			plotresponse.responseratio(datamc, opt,
							types=plot.split('_responseratio_')[0].split('_'),
							over=plot.split('_responseratio_')[1])
		elif 'response' in plot and len(plot.split('_')) > 2:
			plot = plot.replace('bal', 'balresp')
			plotresponse.responseplot(datamc, opt,
							types=plot.split('_response_')[0].split('_'),
							over=plot.split('_response_')[1])
		elif 'ratio' in plot and len(plot.split('_')) > 2:
			plot = plot.replace('bal', 'balresp')
			plotresponse.ratioplot(datamc, opt,
							types=plot.split('_ratio_')[0].split('_'),
							over=plot.split('_ratio_')[1])
		else:
			plot1d.datamcplot(plot, datamc, opt)
