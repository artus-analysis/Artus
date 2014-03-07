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
import Artus.HarryPlotter.tools.getroot as getroot  # read root files
import Artus.HarryPlotter.tools.dictionaries as dictionaries  # this plot dictionary should perhaps move?

# all plot modules
# use ls and imp to read all of them?
import Artus.HarryPlotter.plot1d as plot1d
import Artus.HarryPlotter.plot2d as plot2d
# import ... .Plotting.myhiggsplot as myhiggsplot # to come

def firstpart(inputString):
	pos = inputString.find("_")
	if pos != -1:
		return inputString[:pos]
	else: 
		return inputString


def secondpart(inputString):
	pos = inputString.find("_")
	if pos != -1:
		return inputString[pos+1:]
	else: 
		return inputString

plottypes = ["2D", "1D"]

def determinePlottype(inputString):
	plottype = "1D"
	for plotstring in plottypes:
		if plotstring in inputString:
			plottype = plotstring
			inputString = inputString[inputString.find("_")+1:]

	if firstpart(inputString) == secondpart(inputString):
		if "2D" in plottype:
			print "Unsufficient number of variables."
			sys.exit(1)
		else:
			return plottype, inputString, ""
	else:
		return "2D", firstpart(inputString), secondpart(inputString)




def plot(op):
	"""Search for plots in the module and run them."""
	# dont display any graphics
	gROOT.SetBatch(True)
	startop = copy.deepcopy(op)
	whichfunctions = []
	# specify the plots that will be done depending on the variable selection
	plots = op.plots

	modules = [plot2d, plot1d]

	if op.verbose:
		showoptions(op)

	## place the plotsdict creation somewhere else when working
	plotsdict = {}
	for plot in op.plots:
		plotsdict["plots"] = {}
		plotsdict["plots"][plot] = {}
		plotsdict["plots"][plot]["type"], plotsdict["plots"][plot]["var1"], var2 = determinePlottype(plot)
		if "2D" in plotsdict["plots"][plot]["type"]:
			plotsdict["plots"][plot]["var2"] = var2

	plotsdict["collections"] = {}
	# create list for stacked histograms

	if len(op.files) != len(op.affiliation):
		print "Number of affiliations and number of files do not match!"
		sys.exit()

	for i in range(len(op.files)):
		if not (firstpart(op.affiliation[i]) in plotsdict["collections"]):
			plotsdict["collections"][firstpart(op.affiliation[i])] = {}
		if not (secondpart(op.affiliation[i]) in plotsdict["collections"][firstpart(op.affiliation[i])]):
			plotsdict["collections"][firstpart(op.affiliation[i])][secondpart(op.affiliation[i])] = {}
			plotsdict["collections"][firstpart(op.affiliation[i])][secondpart(op.affiliation[i])]["filenames"] = []
			plotsdict["collections"][firstpart(op.affiliation[i])][secondpart(op.affiliation[i])]["files"] = []
#			plotsdict["collections"][firstpart(op.affiliation[i])][secondpart(op.affiliation[i])]["name"] = secondpart(op.affiliation[i])

		plotsdict["collections"][firstpart(op.affiliation[i])][secondpart(op.affiliation[i])]["filenames"].append(op.files[i])
		plotsdict["collections"][firstpart(op.affiliation[i])][secondpart(op.affiliation[i])]["files"].append(getroot.openfile(op.files[i], op.verbose))
		plotsdict["collections"][firstpart(op.affiliation[i])][secondpart(op.affiliation[i])]["markers"] = op.markers[i]
		if len(op.folder) == 1:
			plotsdict["collections"][firstpart(op.affiliation[i])][secondpart(op.affiliation[i])]["tree"] = op.folder[0]
		else:
			plotsdict["collections"][firstpart(op.affiliation[i])][secondpart(op.affiliation[i])]["tree"] = op.folder[i]

		if len(op.errorbar) == 1:
			plotsdict["collections"][firstpart(op.affiliation[i])][secondpart(op.affiliation[i])]["errorbar"] = op.errorbar[0]
		else:
			plotsdict["collections"][firstpart(op.affiliation[i])][secondpart(op.affiliation[i])]["errorbar"] = op.errorbar[i]


	print plotsdict

	# check input consistency


	if op.list:
		printfunctions(modules)
		sys.exit()

	if op.quantities:
		files = []
		for f in op.files:
			print "Using as file", 1 + op.files.index(f), ":", f
			files += [getroot.openfile(f, op.verbose)]
		printquantities(files, op)
		sys.exit()

	function_selector(plotsdict, op)
	"""
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
	"""

try:
	getobjectfromtree = profile(getobjectfromtree)
except NameError:
	pass  # not running with profiler


# function_selector: takes a list of plots and assigns them to the according funtion
def function_selector(plotsdict, opt):
	for plot in plotsdict["plots"]:

		if plotsdict["plots"][plot]["type"] == "2D":
			plot2d.twoD(plotdict, opt)
		elif plotsdict["plots"][plot]["type"] == "1D":
			plot1d.plot(plot, plotsdict, opt)

		"""elif "_all" in plot:
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
		"""
