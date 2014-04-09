# -*- coding: utf-8 -*-

"""This module selects which plot function to use
- a predefined function from
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import socket
import getpass
import os
import os.path
import sys
import copy
import matplotlib
import json
import argparse
import inspect

from time import localtime, strftime, clock
from ROOT import gROOT, PyConfig
PyConfig.IgnoreCommandLineOptions = True  # prevents Root from reading argv
gROOT.SetBatch(True)  # no root blah blah

# basic functionality
import Artus.HarryPlotter.tools.getroot as getroot
import Artus.HarryPlotter.tools.utils as utils
import Artus.HarryPlotter.tools.labels as labels
import Artus.HarryPlotter.tools.plotrc as plotrc

# all plot modules
import Artus.HarryPlotter.plot1d as plot1d
import Artus.HarryPlotter.plot2d as plot2d
import Artus.HarryPlotter.modules.cutEfficiencyPlots as cutEfficiencyPlots


def get_basic_dictionary():
	"""This function returns a dictionary with all keys needed by HarryPlotter,
	however its values are practically None.
	"""
	
	basedict = {
		'files': [],
		'listfunctions': False,
		'analysismodules': [],
		'writejson': None,
		'readjson': None,
		'root': False,
		'save': True,
		'normalize': False,
		'folder': [],
		
		'roothistos': [],
		'mplhistos': [],
		'rootfiles':[],
		'groups': [],
		
		'x': [],
		'xview': None,
		'y': [],
		'z': [],
		'xbins': [],
		'ybins': [],
		'xlims': None,
		'ylims': None,
		'xname':None,
		'yname':None,
		'zname':None,
		'xticks': None,
		'yticks': None,
		
		'weights': ['1'],
		
		'figure': None,
		'axes': None,
		
		'date':None,
		'author': None,
		'title': None,
		'text': None,
		
		'errorbars': [True],
	# ...
	# ...
	# ...
	}
	return basedict


def get_basic_parser(

		plot='plot',
		out="out",
		formats=['png'],
	
		folder = [],
		files=None,

		labels=[],
		colors=['black', 'cornflowerblue', 'lightgrey'],
		markers=["o", "stack"],
		errorbars=[True, True, True],

		weights=None,

		lumi=None,
		energy=None,
		author=None,
		date=None,
		layout='generic',
		title=None,
		eventnumberlabel=None,
		legloc='best',

		nbins=20,
		rebin=1,
		ratio=False,
		fit=None,

		filename=None,
		backend='mpl',
		plottype = '1D',
	
		text=None,
		readjson=None,
		writejson=None,
		figure=None,
		axes=None,
		x=None, y=None, z=None, xview=None, xbins=[], ybins=[], xname=None,
		yname=None, xticks=None, yticks=None, log=None, xlog=None, xlims=None,
		ylims=None, zname=None, zlims=None, zlog=None,

		save=None,
		verbose=None,
		ratiosubplot=False,
	
		roothistos=None,
		mplhistos=None,
		rootfiles=[],
		groups=None,
		analysismodules=['x'],
		listfunctions=False,
		root=False,
		normalize=False,
		
		userargs = None,
		userArgParsers=None
	
	):
	"""This is the basic parser that reads the Harry options from the command line."""

	if userArgParsers is None:
		userArgParsers = []
	
	parser = argparse.ArgumentParser(epilog="Have fun.",
	                                 fromfile_prefix_chars="@",
	                                 parents=[logger.loggingParser] + userArgParsers)

	#plotname
	parser.add_argument('--plot', '-p', type=str, default=plot,
		help="The name of the plot to be created.")

	# source options
	source = parser.add_argument_group('Source options')
	source.add_argument('-i', '--files', type=str, nargs='*', default=files,
		help="Input root file(s). Specify their affiliation with -a.")
	source.add_argument('--groups', type=str, nargs='*', default = groups,
		help="Group name of input files. Specify the same name to produce stacked plots.")
	source.add_argument('--type', type=str, default = plottype,
		help="Type of plot. Default is %(default)s")

	source.add_argument('--weights', type=str, nargs='+',
		default=weights,
		help='Weight (cut) expressions for creating the histogram from the NTuple. \
			One argument for each input file. If only one argument is given,\
			assume it is the same for each file.')
	source.add_argument('--folder', type=str, nargs='*',
		default=folder,
		help="folder in rootfile. One argument if the same folder should always\
		be used, multiple arguments for a different folder for each input file.")
	source.add_argument('--normalize', '-n', action='store_true',
		default=normalize,
		help="Normalize all histograms to the same number of events.")

	# more general options
	general = parser.add_argument_group('General options')
	general.add_argument('-r', '--rebin', type=int, default=rebin,
		help="Rebinning the histograms with a factor n")
	#TODO:
	#general.add_argument('-R', '--ratio', action='store_true',
	#	help="do a ratio plot from the first two input files")
	general.add_argument('--ratiosubplot', action='store_true',
		help="Add a ratio plot at the bottom.")
	general.add_argument('-F', '--fit', type=str, default=fit,
		help="Do a fit. Options: vertical, chi2, gauss, slope, intercept, quadratic")
	general.add_argument('--nbins', type=int, default=nbins,
		help='number of bins in histogram. Default is %(default)s')

	# output settings
	output = parser.add_argument_group('Output options')
	output.add_argument('-o', '--out', type=str, default=out,
		help="output directory for plots")
	output.add_argument('--backend', type=str, default=backend,
		help="backend for output. Valid options are 'mpl' and 'root'. Default is %(default)s.")
	output.add_argument('-f', '--formats', type=str, nargs='+', default=formats,
		help="output format for the plots.  Default is %(default)s.")
	output.add_argument('--filename', type=str, default=filename,
		help='Specify a filename. By default, the plotname is used')
	output.add_argument('--root', action='store_true', default=root,
		help="Save the created root histograms.")


	# plot labelling
	labelling = parser.add_argument_group('Labelling options')
	labelling.add_argument('-l', '--lumi', type=float, default=lumi,
		help="luminosity for the given data in /fb. Default is %(default)s")
	labelling.add_argument('-e', '--energy', type=int, default=energy,
		help="centre-of-mass energy for the given samples in TeV. Default is %(default)s")
	labelling.add_argument('-A', '--author', type=str, default=author,
		help="author name of the plot")
	labelling.add_argument('--date', type=str, default=date,
		help="show the date in the top left corner. 'iso' is YYYY-MM-DD, " +
			 "'today' is DD Mon YYYY and 'now' is DD Mon YYYY HH:MM.")
	labelling.add_argument('-E', '--eventnumberlabel', action='store_true',
		help="add event number label")
	labelling.add_argument('-t', '--title', type=str, default=title,
		 help="plot title")

	#plot formatting
	formatting = parser.add_argument_group('Formatting options')
	formatting.add_argument('--layout', type=str,
		default='generic',
		help="layout for the plots. E.g. 'document': serif, LaTeX, pdf; " +
			 "'slides': sans serif, big, png; 'generic': slides + pdf. " +
			 "This is not implemented yet.")
	formatting.add_argument('-g', '--legloc', type=str, nargs="?", default=legloc,
		help="Location of the legend. Default is %(default)s. Possible values " +
			 "are keywords like 'lower left' or coordinates like '0.5,0.1'.")
	formatting.add_argument('-C', '--colors', type=str, nargs='+', default=colors,
		help="colors for the plots in the order of the files. Default is: " +
			 ", ".join(colors))
	formatting.add_argument('-L', '--labels', type=str, nargs='+', default=labels,
		help="labels for the plots in the order of the files. Default is: " +
			 ", ".join(labels))
	formatting.add_argument('-m', '--markers', type=str, nargs='+', default=markers,
		help="style for the plot in the order of the files. 'o', 'd', '*', '+' for points, \
			  '-' for lines, 'stack' for filled plots. Default is: %s" % ", ".join(markers))
	formatting.add_argument('--errorbars', default=errorbars, nargs='+',
		help="Include errorbars. Give one argument to count for all curves or one for each input file")
	formatting.add_argument('--text', type=str,
		default=text,
		help='Place a text at a certain location. Syntax is --text="abs" or \
														  --text="abc,0.5,0.9"')
	formatting.add_argument('-G', '--grid', action='store_true', default=False,
		help="Place an axes grid on the plot.")


	# AXIS
	axis = parser.add_argument_group('Axis options')
	axis.add_argument('--log', action='store_true', default=log,
		 help="logarithmic y-axis")
	axis.add_argument('--xlog', action='store_true', default=xlog,
		 help="logarithmic x-axis")
	axis.add_argument('--zlog', action='store_true', default=zlog,
		 help="logarithmic z-axis")
	axis.add_argument('--ylims', type=float, nargs='+', default=ylims,
		help="upper and lower limit for y-axis. Specify four values to also\
			set the y-axis values for a (ratio-)subplot.")
	axis.add_argument('--xlims', type=float, nargs='+', default=xlims,
		help="upper and lower limit for x-axis")
	axis.add_argument('--zlims', type=float, nargs='+', default=zlims,
		help="upper and lower limit for z-axis")
	axis.add_argument('--xview', type=float, nargs='+', default=xview,
		help="upper and lower limit for x-axis VIEWING in the plot")
	axis.add_argument('--xname', type=str, default=xname,
		help='x-axis label name')
	axis.add_argument('--yname', type=str, default=yname,
		help='y-axis label name')
	axis.add_argument('--zname', type=str, default=zname,
		help='z-axis label name')
	axis.add_argument('-x', type=str, default=x, nargs='*',
		help='x-axis variable quantity')
	axis.add_argument('-y', type=str, default=y, nargs='*',
		help='y-axis variable quantity')
	axis.add_argument('-z', type=str, default=z, nargs='*',
		help='z-axis variable quantity')

	axis.add_argument('--xticks', type=float, nargs='+', default=xticks,
		help="add custom xticks")
	axis.add_argument('--yticks', type=float, nargs='+', default=yticks,
		help="add custom yticks")

	# Other options
	group = parser.add_argument_group('Other options')
	#group.add_argument('-v', '--verbose', action='store_true', default=verbose,
	#	help="Increased verbosity")
	group.add_argument('--listfunctions', action='store_true', default=listfunctions,
		help="Show a list of the available functions with docstrings")
	group.add_argument('--quantities', action='store_true',
		help="Show a list of the available quantities/histograms in the NTuple/folder in each file")

	group.add_argument('--writejson', type=str, default=writejson,
		help="Write the plot dictionary into a json file. Argument is (path and) filename,\
			 the .json suffix is added automatically.")
	group.add_argument('--readjson', type=str, default=readjson,
		help="Read the plot dictionary from a json file. Argument is (path and) filename,\
			 the .json suffix is added automatically.")
	group.add_argument('--userargs', type=str, default=userargs, nargs='*',
		help="Additional arguments that can be used optionally in analysis modules.")

	return parser


def create_dictionary_from_parser(parser):
	"""This function creates the Harry-compliant dictionary from an argparse object."""

	opt = parser.parse_args()
	logger.initLogger(opt)

	if opt.listfunctions:
		return opt.__dict__

	opt.subplot = False
	parser.set_defaults(subplot=False)

	opt.brackets = False

	matplotlib.rcParams.update(plotrc.getstyle(opt.layout))

	# get the dicionary from the json file and update the non-default opt values
	if opt.readjson is not None:
		jsondict = readjson(opt.__dict__)
		for i in vars(opt):
			if getattr(opt, i) == parser.get_default(i):
				attr = getattr(opt, i)
				if i in jsondict:
					exec("opt.%s = jsondict[i]" % i)

	if len(opt.x) == 0:
		opt.x = [opt.plot.split("_")[-1]]
	if len(opt.y) == 0 and len(opt.plot.split("_")) > 1:
		opt.y =  [opt.plot.split("_")[-2]]
	if opt.xname == None: 
		opt.xname = opt.x[0]
	if opt.yname == None:
		if 'y' in opt and len(opt.y) == 1:
			opt.yname = opt.y[0]
		else:
			opt.yname = "Entries"
	if opt.filename is None:
		opt.filename = opt.plot
		opt.filename.replace('/', '')
	if opt.groups is None:
		opt.groups = opt.labels

	# We iterate over these lists, therefore we have to make sure they all contain
	# the same number of elements
	primary_iterators = ['folder', 'weights', 'x', 'files']
	iterators = ['errorbars', 'markers', 'labels', 'colors'] + primary_iterators
	maximum = max( [len(getattr(opt, i)) for i in primary_iterators])

	if len(opt.labels) == 0:
		for i in ['x', 'folder', 'weights']:
			if len(getattr(opt, i)) == maximum:
				opt.labels = getattr(opt, i)
	if len(opt.labels) < 1 and len(opt.files) > 0:
		opt.labels = [s.split("/")[-1].split(".")[0] for s in opt.files]

	# if only one folder/... is given, assume we want always the same:
	if maximum > 1:
		for i in iterators:
			if len(getattr(opt, i)) == 1:
				attr = getattr(opt, i) 
				attr *= maximum / len(getattr(opt, i))
			elif len(getattr(opt, i)) < maximum:
				log.warning("Not enough arguments for %s! Only %s arguments provided! We need %s or 1" % (i, len(getattr(opt, i)), maximum))
	if len(opt.groups) == 0:
		opt.groups = opt.labels
	
	opt.markers = [marker.strip() for marker in opt.markers]

	return opt.__dict__


def writejson(plotdict):
	""" Write the plotdict (or any dict) as json."""
	with open("%s.json" % plotdict['writejson'], 'w') as f:
		plotdict['writejson'] = None
		json.dump(plotdict, f, sort_keys=True, indent=4, skipkeys=True)


def readjson(plotdict):
	"""Read the plotdict in from a json file."""
	with open("%s.json" % plotdict['readjson'], "r") as f:
		return json.load(f)

def print_plotdict(plotdict):
	keys = plotdict.keys()
	keys.sort()
	log.debug("Plotdictionary:")
	for key in keys:
		log.debug("  %s: %s" % (key, plotdict[key]))


def plot(plotdict):
	"""This is the main function of HarryPlotter. It processes the plotdict and"""

	if plotdict['writejson'] is not None:
		writejson(plotdict)

	basedict = get_basic_dictionary()
	basedict.update(plotdict)
	plotdict = basedict

	startdict = copy.copy(plotdict)

	print_plotdict(plotdict)

	# add new harrycore modules here:
	modules = [plot2d, plot1d]
	plotdict.setdefault('analysismodules', []).append(cutEfficiencyPlots)
	
	if 'analysismodules' in plotdict:
		if type(plotdict['analysismodules']) == list:
			modules += plotdict['analysismodules']
		else:
			modules += [plotdict['analysismodules']]
			
	# print the list of all available functions
	if plotdict['listfunctions']:
		log.info("\nThe following functions are available in the plotting modules:")
		utils.printfunctions(modules, logging.INFO)
		if not log.isEnabledFor(logging.DEBUG):
			log.info("\nIf you also want to see the available HarryPlotter helper\nfunctions, activate the debug log level")
		log.debug("\nHelper functions:\n")
		utils.printfunctions([utils, getroot, labels, sys.modules[__name__]], logging.DEBUG)
		return

	#open root files
	if plotdict['files']:
		for f in plotdict['files']:
			plotdict['rootfiles'].append(getroot.openfile(f, plotdict['verbose']))

	if plotdict['quantities']:
		utils.printquantities(plotdict)
		return

	function_selector(plotdict)

	# save created root histograms
	if plotdict['root']:
		getroot.save_roothistos(plotdict)

	# check whether the options have changed and how
	if startdict != plotdict:
		log.debug("The following entries have been modified by a plot:")
		for key in plotdict.keys():
			if key in startdict and startdict[key] != plotdict[key]:
				log.debug("  %s\n    previously: %s\n    now:        %s" % (key, startdict[key], plotdict[key]))

try:
	getobjectfromtree = profile(getobjectfromtree)
except NameError:
	pass  # not running with profiler


def function_selector(plotdict):
	"""takes a list of plots and assigns them to the according funtion."""
	if (len(plotdict['analysismodules']) > 0):
		for module in plotdict['analysismodules']:
			if hasattr(module, plotdict['plot']):
				log.info("Calling %s in module %s" % (plotdict['plot'], module.__name__))
				getattr(module, plotdict['plot'])(plotdict)
				return
	if plotdict["type"] == "2D":
		plot2d.plot_2d(plotdict)
	elif plotdict["type"] == "1D":
		plot1d.plot1d(plotdict)
