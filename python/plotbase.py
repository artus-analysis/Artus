# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import hashlib
import os
import re

import Artus.HarryPlotter.processor as processor
import Artus.HarryPlotter.roottools as roottools
import Artus.HarryPlotter.modules.eventselectionoverlap as eventselectionoverlap
import Artus.HarryPlotter.extrafunctions as extrafunctions

class PlotBase(processor.Processor):
	
	def __init__(self):
		super(PlotBase, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(PlotBase, self).modify_argument_parser(parser, args)
		
		# plotting settings
		self.plotting_options = parser.add_argument_group("Plotting options")
		self.plotting_options.add_argument("--ratio", default=False, action="store_true",
		                                   help="Show ratio subplot.")
		self.plotting_options.add_argument("--ratio-num", nargs="+",
		                                   help="Nick names for numerators of ratio. Multiple nicks in one argument (ws-separated) are summed. [Default: first nick]")
		self.plotting_options.add_argument("--ratio-denom", nargs="+",
		                                   help="Nick names for denominators of ratio. Multiple nicks in one argument (ws-separated) are summed. [Default: all but first nick]")
		
		# axis settings
		self.axis_options = parser.add_argument_group("Axis options")
		self.axis_options.add_argument("--x-lims", type=float, nargs=2,
		                               help="Lower and Upper limit for x-axis.")
		self.axis_options.add_argument("--x-label", type=str,
		                               help="X-axis label name.")
		self.axis_options.add_argument("--x-log", action="store_true", default=False,
		                               help="Logarithmic x-axis.")
		self.axis_options.add_argument("--x-ticks", type=float, nargs="+",
		                               help="Custom ticks for the X-axis")
		self.axis_options.add_argument("--x-tick-labels", type=float, nargs="+",
		                               help="Custom tick labels for the X-axis")
		#self.axis_options.add_argument("--xview", type=float, nargs=2, default=xview,
		#                               help="Lower and Upper limit for x-axis viewing in the plot")
		
		self.axis_options.add_argument("--y-lims", type=float, nargs=2,
		                               help="Lower and Upper limit for y-axis.")
		self.axis_options.add_argument("--y-ratio-lims", type=float, nargs=2,
		                               help="Lower and Upper limit for y-axis of a possible ratio subplot.")
		self.axis_options.add_argument("--y-label", type=str, default="Events",
		                               help="Y-axis label name. [Default: %(default)s]")
		self.axis_options.add_argument("--y-ratio-label", type=str, default="Ratio",
		                               help="Y-axis label name of a possible ratio subplot. [Default: %(default)s]")
		self.axis_options.add_argument("--y-log", action="store_true", default=False,
		                               help="Logarithmic y-axis.")
		self.axis_options.add_argument("--y-ticks", type=float, nargs="+",
		                               help="Custom ticks for the Y-axis")
		self.axis_options.add_argument("--y-tick-labels", type=float, nargs="+",
		                               help="Custom tick labels for the Y-axis")
		
		self.axis_options.add_argument("--z-lims", type=float, nargs=2,
		                               help="Lower and Upper limit for z-axis.")
		self.axis_options.add_argument("--z-label", type=str, default="Events",
		                               help="Z-axis label name. [Default: %(default)s]")
		self.axis_options.add_argument("--z-log", action="store_true", default=False,
		                               help="Logarithmic z-axis.")
		self.axis_options.add_argument("--z-ticks", type=float, nargs="+",
		                               help="Custom ticks for the Z-axis")
		self.axis_options.add_argument("--z-tick-labels", type=float, nargs="+",
		                               help="Custom tick labels for the Z-axis")

		#plot formatting
		self.formatting_options = parser.add_argument_group("Formatting options")
		self.formatting_options.add_argument("-C", "--colors", type=str, nargs="+",
		                                     help="Colors for the plots.")
		self.formatting_options.add_argument("--ratio-colors", type=str, nargs="+",
		                                     help="Colors for the ratio subplots.")
		self.formatting_options.add_argument("-L", "--labels", type=str, nargs="+",
		                                     help="Labels for the plots.")
		self.formatting_options.add_argument("--ratio-labels", type=str, nargs="+",
		                                     help="Labels for the ratio subplots.")
		self.formatting_options.add_argument("-m", "--markers", type=str, nargs="+",
		                                     help="Style for the plots.")
		self.formatting_options.add_argument("--ratio-markers", type=str, nargs="+",
		                                     help="Style for the ratio subplots.")
		self.formatting_options.add_argument("--errorbars", type=bool, nargs="+",
		                                     help="Errorbars for the plots. [Default: True for first plot, False otherwise]")
		self.formatting_options.add_argument("--ratio-errorbars", type=bool, nargs="+", default=[True],
		                                     help="Errorbars for the ratio subplots. [Default: True]")
		self.formatting_options.add_argument("-g", "--legloc", type=str, nargs="?",
		                                     help="Location of the legend.")
		self.formatting_options.add_argument("-G", "--grid", action="store_true", default=False,
		                                     help="Place an axes grid on the plot.")
		self.formatting_options.add_argument("--ratio-grid", action="store_true", default=False,
		                                     help="Place an axes grid on the ratio subplot.")
		self.formatting_options.add_argument("--stack", type=str, nargs="+",
		                                     help="Defines nick names for stacking. Inputs with the same nick name will be stacked. By default, every input gets a unique nick name.")

		# plot labelling
		self.labelling_options = parser.add_argument_group("Labelling options")
		self.labelling_options.add_argument("-t", "--title", type=str,
		                                    help="Plot title")
		self.labelling_options.add_argument("-l", "--lumi", type=float,
		                                    help="Luminosity for the given data in /fb.")
		self.labelling_options.add_argument("-e", "--energy", type=str, nargs="+",
		                                    help="Centre-of-mass energy for the given samples in TeV.")
		self.labelling_options.add_argument("-A", "--author", type=str,
		                                    help="author name of the plot")
		self.labelling_options.add_argument("--date", type=str,
		                                    help="Show the date in the top left corner. \"iso\" is YYYY-MM-DD, \"today\" is DD Mon YYYY and \"now\" is DD Mon YYYY HH:MM.")
		self.labelling_options.add_argument("-E", "--event-number-label", action="store_true", default=False,
		                                    help="Add event number label")
		self.formatting_options.add_argument("--text", type=str,
		                                     help="Place a text at a certain location. Syntax is --text=\"abc\" or --text=\"abc,0.5,0.9\"")
		
		# output settings
		self.output_options = parser.add_argument_group("Output options")
		self.output_options.add_argument("-o", "--output-dir", default="plots",
		                                 help="Output directory for plots. [Default: %(default)s]")
		self.output_options.add_argument("--filename",
		                                 help="Filename of the plot without extension. By default, it is constructed from the x/y/z expressions.")
		self.output_options.add_argument("-f", "--formats", nargs="+", default=["png"],
		                                 help="Format of the plots. [Default: %(default)s]")

		# settings to increase usability
		self.other_options = parser.add_argument_group("Other features")
		self.other_options.add_argument("--quantities", action="store_true", default=False,
		                                 help="Print available quantities in given folder")
		self.other_options.add_argument("--live", default=None, nargs='?', const='gthumb',
		                                 help="Open plot in viewer after its creation. Parameter is the name of your desired viewer.")

		# module specific settings # TODO
		if eventselectionoverlap.EventSelectionOverlap.name() in args["analysis_modules"]:
			parser.set_defaults(x_label="Event Selection Overlap")
	
	def prepare_args(self, parser, plotData):
		super(PlotBase, self).prepare_args(parser, plotData)


		# prepare nick names for ratio subplot
		if plotData.plotdict["ratio_num"] == None: plotData.plotdict["ratio_num"] = [plotData.plotdict["nicks"][0]]
		if plotData.plotdict["ratio_denom"] == None: plotData.plotdict["ratio_denom"] = [" ".join(plotData.plotdict["nicks"][1:])]
		problems_with_ratio_nicks = False
		for ratio_nicks_key in ["ratio_num", "ratio_denom"]:
			plotData.plotdict[ratio_nicks_key] = [nicks.split() for nicks in plotData.plotdict[ratio_nicks_key]]
			for nicks in plotData.plotdict[ratio_nicks_key]:
				for nick in nicks:
					if nick not in plotData.plotdict["nicks"]:
						log.warning("Nick name \"%s\" of argument --%s does not exist in argument --nicks!" % (nick, ratio_nicks_key.replace("_", "-")))
						problems_with_ratio_nicks = True
		if problems_with_ratio_nicks:
			log.warning("Continue without ratio subplot!")
			plotData.plotdict["ratio"] = False
		self.prepare_list_args(plotData, ["ratio_num", "ratio_denom", "ratio_colors", "ratio_labels", "ratio_markers", "ratio_errorbars"])
		
		# construct labels from x/y/z expressions if not specified by user
		for labelKey, expressionKey in zip(["x_label", "y_label", "z_label"],
		                                   ["x_expressions", "y_expressions", "z_expressions"]):
			if plotData.plotdict[labelKey] == None:
				plotData.plotdict[labelKey] = reduce(lambda a, b: "%s, %s" % (str(a), str(b)), set(plotData.plotdict[expressionKey]))
			if plotData.plotdict[labelKey] == None: plotData.plotdict[labelKey] = ""

		# write name of output file in dictionary
		plotData.plotdict["output_filenames"] = []
		for plot_format in plotData.plotdict["formats"]:
			plotData.plotdict["output_filenames"].append(os.path.join(plotData.plotdict["output_dir"], plotData.plotdict["filename"]+"."+plot_format))

		# formatting options
		self.prepare_list_args(plotData, ["colors", "labels", "markers", "errorbars", "stack"], len(plotData.plotdict["root_histos"]))
		
		for index, errorbar in enumerate(plotData.plotdict["errorbars"]):
			if errorbar == None:
				plotData.plotdict["errorbars"][index] = True if index == 0 else False
		
		# create output directory if not exisiting
		if not os.path.exists(plotData.plotdict["output_dir"]):
			os.makedirs(plotData.plotdict["output_dir"])
			log.info("Created output directory \"%s\"." % plotData.plotdict["output_dir"])
		
		# construct file name from x/y/z expressions if not specified by user
		if plotData.plotdict["filename"] == None:
			filename = ""
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

		# prepare nicknames for stacked plots

		stack = plotData.plotdict["stack"]
		plotData.plotdict["stack"] = [stack if stack != None else ("stack%d" % index) for index, stack in enumerate(plotData.plotdict["stack"])]
		
		# module specific settings # TODO
		if plotData.plotdict["analysis_modules"] != None and eventselectionoverlap.EventSelectionOverlap.name() in plotData.plotdict["analysis_modules"] and plotData.plotdict["x_tick_labels"] == None:
			labels = filter(lambda label: label != None, plotData.plotdict.pop("labels"))
			plotData.plotdict["labels"] = [None]
			if len(labels) < 2:
				log.warning("Argument --labels needs 2 values! These are filled up with the nick names.")
				labels += plotData.plotdict.pop("nicks")
			plotData.plotdict["x_tick_labels"] = [
				"Only " + labels[0],
				"Intersection",
				"Only " + labels[1],
			]
	
	def run(self, plotData):
		super(PlotBase, self).run(plotData)
		
		self.calculate_ratios(plotData)
		self.create_canvas(plotData)
		self.prepare_histograms(plotData)
		self.make_plots(plotData)
		self.modify_axes(plotData)
		self.add_labels(plotData)
		self.add_texts(plotData)
		self.save_canvas(plotData)
		self.plot_end(plotData)

	def calculate_ratios(self, plotData):
		if plotData.plotdict["ratio"]:
			for numerator_nicks, denominator_nicks in zip(plotData.plotdict["ratio_num"],
			                                              plotData.plotdict["ratio_denom"]):
				name = hashlib.md5("_".join(numerator_nicks+denominator_nicks)).hexdigest()
				numerator_histogram = roottools.RootTools.add_root_histograms(*[plotData.plotdict["root_histos"][nick] for nick in numerator_nicks],
				                                                              name=name+"_numerator")
				denominator_histogram = roottools.RootTools.add_root_histograms(*[plotData.plotdict["root_histos"][nick] for nick in denominator_nicks],
				                                                                name=name+"_denominator")
				ratio_histogram = numerator_histogram.Clone(name + "_ratio")
				ratio_histogram.Divide(denominator_histogram)
				plotData.plotdict.setdefault("root_ratio_histos", []).append(ratio_histogram)
	
	def create_canvas(self, plotData):
		pass
	
	def prepare_histograms(self, plotData):
		pass
	
	def make_plots(self, plotData):
		pass
	
	def modify_axes(self, plotData):
		pass
	
	def add_labels(self, plotData):
		pass
	
	def add_texts(self, plotData):
		pass
	
	def save_canvas(self, plotData):
		pass

	def plot_end(self, plotData):
		if not (plotData.plotdict["live"]==None) :
			for output_filename in plotData.plotdict["output_filenames"]:
				extrafunctions.show_plot(output_filename, plotData.plotdict["live"])


	def set_default_ratio_colors(self, plotData):
		
		# defaults for colors
		for index, color_ratio_color in enumerate(zip(plotData.plotdict["colors"],
		                                              plotData.plotdict["ratio_colors"])):
			color, ratio_color = color_ratio_color
			if ratio_color == None:
				plotData.plotdict["ratio_colors"][index] = plotData.plotdict["colors"][index] # TODO
