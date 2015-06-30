# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import datetime
import errno
import hashlib
import numpy
import os
import pprint
import re
import sys

import ROOT

import Artus.HarryPlotter.processor as processor
import Artus.Utility.tools as tools
import Artus.HarryPlotter.utility.roottools as roottools
import Artus.HarryPlotter.utility.colors as colors

class PlotBase(processor.Processor):
	
	def __init__(self):
		super(PlotBase, self).__init__()
		
		self.predefined_colors = None
	
	def modify_argument_parser(self, parser, args):
		super(PlotBase, self).modify_argument_parser(parser, args)
		
		# plotting settings
		self.plotting_options = parser.add_argument_group("Plotting options")
		self.plotting_options.add_argument("--nicks-whitelist", nargs="+", default=[],
		                                   help="Whitelist of (regexp) nick names for objects to be plotted. This also allows for redefining the order of nicks for the plotting. Use \"^nick$\" for requirering exact matches.")
		self.plotting_options.add_argument("--nicks-blacklist", nargs="+", default=["noplot"],
		                                   help="Blacklist of (regexp) nick names for objects to be excluded from plotting. Use \"^nick$\" for requirering exact matches. [Default: %(default)s]")
		self.plotting_options.add_argument("--subplot-nicks", nargs="+", default=["ratio"],
		                                   help="List of (regexp) nick names for objects to be plotted in the subplot. Use \"^nick$\" for requirering exact matches. [Default: %(default)s]")
		# axis settings
		self.axis_options = parser.add_argument_group("Axis options")
		self.axis_options.add_argument("--x-lims", type=float, nargs=2,
		                               help="Lower and Upper limit for x-axis.")
		self.axis_options.add_argument("--x-label", type=str,
		                               help="X-axis label name.")
		self.axis_options.add_argument("--x-log", nargs="?", type="bool", default=False, const=True,
		                               help="Logarithmic x-axis. [Default: %(default)s]")
		self.axis_options.add_argument("--x-ticks", type=float, nargs="+",
		                               help="Custom ticks for the X-axis")
		self.axis_options.add_argument("--x-tick-labels", type=str, nargs="+",
		                               help="Custom tick labels for the X-axis")
		
		self.axis_options.add_argument("--y-lims", type=float, nargs=2,
		                               help="Lower and Upper limit for y-axis.")
		self.axis_options.add_argument("--y-subplot-lims", type=float, nargs=2,
		                               help="Lower and Upper limit for y-axis of a possible subplot.")
		self.axis_options.add_argument("--y-label", type=str, default="Events",
		                               help="Y-axis label name. [Default: %(default)s]")
		self.axis_options.add_argument("--y-subplot-label", type=str, default="Ratio",
		                               help="Y-axis label name of a possible subplot. [Default: %(default)s]")
		self.axis_options.add_argument("--y-log", nargs="?", type="bool", default=False, const=True,
		                               help="Logarithmic y-axis. [Default: %(default)s]")
		self.axis_options.add_argument("--y-ticks", type=float, nargs="+",
		                               help="Custom ticks for the Y-axis")
		self.axis_options.add_argument("--y-tick-labels", type=str, nargs="+",
		                               help="Custom tick labels for the Y-axis")
		
		self.axis_options.add_argument("--z-lims", type=float, nargs=2,
		                               help="Lower and Upper limit for z-axis.")
		self.axis_options.add_argument("--z-label", type=str, default="Events",
		                               help="Z-axis label name. [Default: %(default)s]")
		self.axis_options.add_argument("--z-log", nargs="?", type="bool", default=False, const=True,
		                               help="Logarithmic z-axis. [Default: %(default)s]")
		self.axis_options.add_argument("--z-ticks", type=float, nargs="+",
		                               help="Custom ticks for the Z-axis")
		self.axis_options.add_argument("--z-tick-labels", type=str, nargs="+",
		                               help="Custom tick labels for the Z-axis")

		#plot formatting
		self.formatting_options = parser.add_argument_group("Formatting options")
		self.formatting_options.add_argument("--color-scheme", default="default",
		                                     help="Color scheme for predefined colors. [Default: '%(default)s']")
		self.formatting_options.add_argument("-C", "--colors", type=str, nargs="+",
		                                     help="Colors for the plots.")
		self.formatting_options.add_argument("--colormap", nargs="?",
		                                     help="Colormap. [Default: '%(default)s']")
		self.formatting_options.add_argument("-L", "--labels", type=str, nargs="+",
		                                     help="Labels for the plots.")
		self.formatting_options.add_argument("-m", "--markers", type=str, nargs="+",
		                                     help="Style for the plots.")
		self.formatting_options.add_argument("--line-styles", nargs="+",
                                             help="Line style of plots line. [Default: %(default)s]")
		self.formatting_options.add_argument("--line-widths", nargs="+", type=int,
		                                     help="Line width of plots line. [Default: %(default)s]")
		self.formatting_options.add_argument("--legend", type=str, nargs="?",
		                                     help="Location of the legend. Use 'None' to not set any legend")
		self.formatting_options.add_argument("--legend-cols", type=int, default=1,
		                                     help="Number of columns in the legend. [Default: %(default)s]")
		self.formatting_options.add_argument("-G", "--grid", nargs="?", type="bool", default=False, const=True,
		                                     help="Place an axes grid on the plot. [Default: %(default)s]")
		self.formatting_options.add_argument("--subplot-grid", nargs="?", type="bool", default=False, const=True,
		                                     help="Place an axes grid on the subplot. [Default: %(default)s]")
		self.formatting_options.add_argument("--stacks", type=str, nargs="+", default=[None],
		                                     help="Defines nick names for stacking. Inputs with the same nick name will be stacked. By default, every input gets a unique nick name. [Default: %(default)s]")
		self.formatting_options.add_argument("--lines", type=float, nargs="+", default=[],
		                                     help="Place auxiliary lines at given y-values.")
		self.formatting_options.add_argument("--subplot-lines", nargs="+", type=float, default=[1.],
		                                     help="Place auxiliary lines on the subplot at given y-values.")

		# plot labelling
		self.labelling_options = parser.add_argument_group("Labelling options")
		self.labelling_options.add_argument("-t", "--title", type=str,
		                                    help="Plot title")
		self.labelling_options.add_argument("-l", "--lumis", type=float, nargs="+",
		                                    help="Luminosities for the given data in fb^(-1).")
		self.labelling_options.add_argument("-e", "--energies", type=float, nargs="+",
		                                    help="Centre-of-mass energies for the given samples in TeV.")
		self.labelling_options.add_argument("-A", "--author", type=str,
		                                    help="author name of the plot")
		self.labelling_options.add_argument("--date", type=str,
		                                    help="Show the date in the top left corner. \"iso\" is YYYY-MM-DD, \"today\" is DD Mon YYYY and \"now\" is DD Mon YYYY HH:MM.")
		self.labelling_options.add_argument("-E", "--event-number-label", nargs="?", type="bool", default=False, const=True,
		                                    help="Add event number label. [Default: %(default)s]")
		self.formatting_options.add_argument("--texts", type=str, nargs="+", default=[None],
		                                     help="Place text(s) on the plot.")
		self.formatting_options.add_argument("--texts-y", type=float, nargs="+", default=[0.8],
		                                     help="Y-coordinate(s) of text(s) on plot. Ranges from 0 to 1. [Default: %(default)s]")
		self.formatting_options.add_argument("--texts-x", type=float, nargs="+", default=[0.5],
		                                     help="X-coordinate(s) of text(s) on plot. Ranges from 0 to 1. [Default: %(default)s]")
		self.formatting_options.add_argument("--texts-size", type=float, nargs="+", default=[None],
		                                     help="Font size for the text labels. [Default: %(default)s]")
		
		# output settings
		self.output_options = parser.add_argument_group("Output options")
		self.output_options.add_argument("-o", "--output-dir", default="plots",
		                                 help="Output directory for plots. [Default: %(default)s]")
		self.output_options.add_argument("--filename",
		                                 help="Filename of the plot without extension. By default, it is constructed from the x/y/z expressions.")
		self.output_options.add_argument("--formats", nargs="+", default=["png"],
		                                 help="Format of the plots. [Default: %(default)s]")

		# settings to increase usability
		self.other_options = parser.add_argument_group("Other features")
		self.other_options.add_argument("--live", default=None, nargs='?', const='gnome-open',
		                                 help="Open plot in viewer after its creation. Parameter is the name of your desired viewer.")
		self.other_options.add_argument("--userpc", nargs="?", type="bool", default=False, const=True,
		                                 help="If 'live' is enabled, the image will be copied to the user desktop (via ssh) and the image viewer will be started on the user desktop with this option. [Default: %(default)s]")
		self.other_options.add_argument("--dict", nargs="?", type="bool", default=False, const=True,
		                                 help="Print out plot dictionary when plotting. [Default: %(default)s]")
		self.other_options.add_argument("--www", type=str, default=None, nargs='?', const="",
		                                 help="""Push output plots directly to your public EKP webspace.
		                                 Default location is http://www-ekp.physik.uni-karlsruhe.de/~<USER>/plots_archive/<DATE>
		                                 Optional argument is the name of a subdirectory that will appended to the default location.""")
	
	def prepare_args(self, parser, plotData):
		super(PlotBase, self).prepare_args(parser, plotData)
		
		if self.predefined_colors is None:
			self.predefined_colors = colors.ColorsDict(color_scheme=plotData.plotdict["color_scheme"])
		
		# delete nicks that do not need to be used for plotting
		self.select_histograms(plotData)
		
		# construct labels from x/y/z expressions if not specified by user
		for label_key, expression_key in zip(["x_label", "y_label", "z_label"],
		                                   ["x_expressions", "y_expressions", "z_expressions"]):
			if plotData.plotdict[label_key] == None:
				if plotData.plotdict["input_modules"] == ["InputInteractive"]:
					plotData.plotdict[label_key] = str(label_key[1])
				elif expression_key in plotData.plotdict:
					plotData.plotdict[label_key] = reduce(lambda a, b: "%s, %s" % (str(a), str(b)), set(plotData.plotdict.get(expression_key, [""])))
			if plotData.plotdict[label_key] == None:
				plotData.plotdict[label_key] = ""
			# if y/z-expressions exist, we dont want the axis to be labelled "Events"
			if (expression_key in plotData.plotdict
						and list(set([x if x is None else str(x) for x in plotData.plotdict[expression_key]])) != [None]
						and len(list(set([x if x is None else str(x) for x in plotData.plotdict[expression_key]]))) == 1
						and plotData.plotdict[label_key] == "Events"):
				plotData.plotdict[label_key] = list(set([x if x is None else str(x) for x in plotData.plotdict[expression_key]]))[0]

		# formatting options
		if plotData.plotdict["labels"] == None or all([i == None for i in plotData.plotdict["labels"]]):
			plotData.plotdict["labels"] = plotData.plotdict["nicks"]
		
		self.prepare_list_args(plotData, ["nicks", "colors", "labels", "markers", "line_styles", "line_widths"],
				n_items = max([len(plotData.plotdict[l]) for l in ["nicks", "stacks"] if plotData.plotdict[l] is not None]))
		# stacks are expanded by appending None's
		plotData.plotdict["stacks"] = plotData.plotdict["stacks"]+[None]*(len(plotData.plotdict["nicks"])-len(plotData.plotdict["stacks"]))
		
		plotData.plotdict["colors"] = [None if color is None else self.predefined_colors.get_predefined_color(color) for color in plotData.plotdict["colors"]]
		
		if plotData.plotdict["www"] != None:
			plotData.plotdict["output_dir"] = "/".join(['websync', datetime.date.today().strftime('%Y_%m_%d'), (plotData.plotdict["www"] or "")])
		# create output directory if not exisiting
		try:
			os.makedirs(plotData.plotdict["output_dir"])
			log.info("Created output directory \"%s\"." % plotData.plotdict["output_dir"])
		except OSError as exc:
			# if target directory already exists, ignore exception:
			if exc.errno == errno.EEXIST and os.path.isdir(plotData.plotdict["output_dir"]):
				pass
			else: raise
		
		# construct file name from x/y/z expressions if not specified by user
		if plotData.plotdict["filename"] == None:
			filename = ""
			if plotData.plotdict["input_modules"] == ["InputInteractive"]:
				filename = "plot"
			else:
				for expressions in [plotData.plotdict["z_expressions"],
					                plotData.plotdict["y_expressions"],
					                plotData.plotdict["x_expressions"]]:
					expression_string = reduce(lambda a, b: "%s__%s" % (str(a), str(b)), set(expressions))
					if expression_string == None:
						expression_string = "None"
					expression_string = re.sub("[^a-zA-Z0-9]", "_", expression_string)
					if expression_string != "None":
						if len(filename) > 0:
							filename += "_VS_"
						filename += expression_string
			plotData.plotdict["filename"] = filename

		# write name of output file in dictionary
		plotData.plotdict["output_filenames"] = []
		for plot_format in plotData.plotdict["formats"]:
			plotData.plotdict["output_filenames"].append(os.path.join(plotData.plotdict["output_dir"], plotData.plotdict["filename"]+"."+plot_format))
		
		if plotData.plotdict["export_json"] == "default":
			plotData.plotdict["export_json"] = os.path.join(plotData.plotdict["output_dir"], plotData.plotdict["filename"]+".json")

		# prepare nicknames for stacked plots
		plotData.plotdict["stacks"] = [stack if stack != None else ("stack%d" % index) for index, stack in enumerate(plotData.plotdict["stacks"])]

		# prepare arguments for text label(s)
		if plotData.plotdict["texts"] is not None:
			self.prepare_list_args(plotData, ["texts", "texts_y", "texts_x", "texts_size"])
			if len(plotData.plotdict["texts"]) == 1 and plotData.plotdict["texts"][0] is None:
				plotData.plotdict["texts"] = []
				plotData.plotdict["texts_x"] = []
				plotData.plotdict["texts_y"] = []
				plotData.plotdict["texts_size"] = []

		if plotData.plotdict["legend"] == "None":
			plotData.plotdict["legend"] = None
		for key in ['lumis', 'energies']:
			if plotData.plotdict[key] is not None and all([item in [0, "None"] for item in plotData.plotdict[key]]):
				plotData.plotdict[key] = None

	def run(self, plotData):
		super(PlotBase, self).run(plotData)
		
		self.set_style(plotData)
		self.create_canvas(plotData)
		self.prepare_histograms(plotData)
		self.determine_plot_lims(plotData)
		self.make_plots(plotData)
		self.modify_axes(plotData)
		self.add_grid(plotData)
		self.add_labels(plotData)
		self.add_texts(plotData)
		self.plot_end(plotData)

	def select_histograms(self, plotData):
		sorted_nicks_to_keep = []
		
		# handle regexps in white/black lists for nicks
		plotData.plotdict["nicks"] = tools.matching_sublist(
				plotData.plotdict["nicks"],
				plotData.plotdict["nicks_whitelist"],
				plotData.plotdict["nicks_blacklist"]
		)
		log.debug("Final order of object nicks for plotting: %s" % ", ".join(plotData.plotdict["nicks"]))
		if len(plotData.plotdict["nicks"]) == 0:
			log.critical("No (remaining) objects to be plotted!")
			sys.exit(1)
		
		# handle subplot regexps
		plotData.plotdict["subplot_nicks"] = tools.matching_sublist(
				plotData.plotdict["nicks"],
				plotData.plotdict["subplot_nicks"]
		)
		log.debug("Object nicks for the subplot: %s" % ", ".join(plotData.plotdict["subplot_nicks"]))
		plotData.plotdict["subplots"] = [nick in plotData.plotdict["subplot_nicks"] for nick in plotData.plotdict["nicks"]]

	def set_style(self, plotData):
		pass
	
	def create_canvas(self, plotData):
		pass
	
	def prepare_histograms(self, plotData):
		# handle stacks
		# TODO: define how functions should act when stacked
		for index, (nick1, stack1) in enumerate(zip(plotData.plotdict["nicks"], plotData.plotdict["stacks"])):
			for nick2, stack2 in zip(plotData.plotdict["nicks"], plotData.plotdict["stacks"])[:index]:
				if stack1 == stack2:
					# TProfiles cannot be added, convert to TH1
					if isinstance(plotData.plotdict["root_objects"][nick2], ROOT.TProfile):
						plotData.plotdict["root_objects"][nick2] = plotData.plotdict["root_objects"][nick2].ProjectionX()
					if isinstance(plotData.plotdict["root_objects"][nick1], ROOT.TProfile):
						plotData.plotdict["root_objects"][nick1] = plotData.plotdict["root_objects"][nick1].ProjectionX()
					plotData.plotdict["root_objects"][nick2].Add(plotData.plotdict["root_objects"][nick1])

			# some debug infos
			if log.isEnabledFor(logging.DEBUG):
				log.debug("\nContents of nick %s, stack %s:" % (nick1, stack1))
				plotData.plotdict["root_objects"][nick1].Print("range")
		
		# remove underflow/overflow bin contents
		for nick in plotData.plotdict["nicks"]:
			root_object = plotData.plotdict["root_objects"][nick]
			if isinstance(root_object, ROOT.TH1):
				if root_object.GetDimension() == 1:
					root_object.SetBinContent(0, 0.0)
					root_object.SetBinError(0, 0.0)
					root_object.SetBinContent(root_object.GetNbinsX()+1, 0.0)
					root_object.SetBinError(root_object.GetNbinsX()+1, 0.0)
				else:
					# TODO: iterate over multidim. under-/overflow bins
					pass

	def determine_plot_lims(self, plotData):
		self.x_min = None
		self.x_max = None
		self.y_min = None
		self.y_max = None
		self.z_min = None
		self.z_max = None
		self.max_dim = 2
		
		self.y_sub_min = None
		self.y_sub_max = None
		self.z_sub_min = None
		self.z_sub_max = None
		self.max_sub_dim = 2
		
		for nick, subplot in zip(plotData.plotdict["nicks"], plotData.plotdict["subplots"]):
			plot_x_min, plot_x_max, plot_y_min, plot_y_max, plot_z_min, plot_z_max, max_dim = PlotBase.get_plot_lims(
					plotData.plotdict["root_objects"][nick],
					x_log=plotData.plotdict["x_log"],
					y_log=plotData.plotdict["y_log"],
					z_log=plotData.plotdict["z_log"]
			)
			
			self.x_min, self.x_max = PlotBase.update_lims(self.x_min, self.x_max, plot_x_min, plot_x_max)
			if subplot == True:
				self.max_sub_dim = max_dim
				self.y_sub_min, self.y_sub_max = PlotBase.update_lims(self.y_sub_min, self.y_sub_max, plot_y_min, plot_y_max)
				self.z_sub_min, self.z_sub_max = PlotBase.update_lims(self.z_sub_min, self.z_sub_max, plot_z_min, plot_z_max)
			else:
				self.max_dim = max_dim
				self.y_min, self.y_max = PlotBase.update_lims(self.y_min, self.y_max, plot_y_min, plot_y_max)
				self.z_min, self.z_max = PlotBase.update_lims(self.z_min, self.z_max, plot_z_min, plot_z_max)

	def make_plots(self, plotData):
		pass
	
	def modify_axes(self, plotData):
		pass
	
	def add_grid(self, plotData):
		pass
	
	def add_labels(self, plotData):
		pass
	
	def add_texts(self, plotData):
		self.dataset_title = ""
		run_periods = []
		# if lumi and energy are available:
		if (not plotData.plotdict["lumis"] is None) and (not plotData.plotdict["energies"] is None):
			for lumi, energy in zip(plotData.plotdict["lumis"], plotData.plotdict["energies"]):
				run_periods.append("%s \,\mathrm{fb}^{-1} (%s \,TeV)" % (str(lumi), str(int(energy))))
		# if only energy is available (MC-plots):
		elif (not plotData.plotdict["energies"] is None):
			for energy in plotData.plotdict["energies"]:
				run_periods.append("\sqrt{s} = %s \,TeV" % str(int(energy)))
		if len(run_periods) > 0:
			self.dataset_title = "$" + (" + ".join(run_periods)) + "$"
	
	def plot_end(self, plotData):
		if plotData.plotdict["dict"]:
			pprint.pprint(plotData.plotdict)

	@staticmethod
	def get_plot_lims(root_object, x_log=False, y_log=False, z_log=False):
		max_dim = roottools.RootTools.get_dimension(root_object)
		
		x_min, x_max = roottools.RootTools.get_min_max(root_object, 0)
		if x_log and (x_min * x_max <= 0.0):
			x_min, x_max = roottools.RootTools.get_min_max(root_object, 0, lower_threshold=0.0)
		
		y_min, y_max = roottools.RootTools.get_min_max(root_object, 1)
		if y_log and (y_min * y_max <= 0.0):
			y_min, y_max = roottools.RootTools.get_min_max(root_object, 1, lower_threshold=0.0)
		
		z_min, z_max = None, None
		if max_dim > 2:
			z_min, z_max = roottools.RootTools.get_min_max(root_object, 2)
			if z_log and (z_min * z_max <= 0.0):
				z_min, z_max = roottools.RootTools.get_min_max(root_object, 2, lower_threshold=0.0)
		
		return x_min, x_max, y_min, y_max, z_min, z_max, max_dim
	
	@staticmethod
	def update_lims(min_1, max_1, min_2, max_2):
		result_min = None
		if min_2 is None:
			result_min = min_1
		elif min_1 is None:
			result_min = min_2
		else:
			result_min = min(min_1, min_2)
		
		result_max = None
		if max_2 is None:
			result_max = max_1
		elif max_1 is None:
			result_max = max_2
		else:
			result_max = max(max_1, max_2)
		
		return result_min, result_max
