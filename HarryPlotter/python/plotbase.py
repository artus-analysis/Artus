# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import datetime
import hashlib
import os
import pprint
import re
import subprocess

import ROOT

import Artus.HarryPlotter.processor as processor
import Artus.Utility.tools as tools
import Artus.HarryPlotter.utility.roottools as roottools
import Artus.HarryPlotter.utility.extrafunctions as extrafunctions

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
		self.axis_options.add_argument("--x-log", action="store_true", default=False,
		                               help="Logarithmic x-axis.")
		self.axis_options.add_argument("--x-ticks", type=float, nargs="+",
		                               help="Custom ticks for the X-axis")
		self.axis_options.add_argument("--x-tick-labels", type=str, nargs="+",
		                               help="Custom tick labels for the X-axis")
		#self.axis_options.add_argument("--xview", type=float, nargs=2, default=xview,
		#                               help="Lower and Upper limit for x-axis viewing in the plot")
		
		self.axis_options.add_argument("--y-lims", type=float, nargs=2,
		                               help="Lower and Upper limit for y-axis.")
		self.axis_options.add_argument("--y-subplot-lims", type=float, nargs=2,
		                               help="Lower and Upper limit for y-axis of a possible subplot.")
		self.axis_options.add_argument("--y-label", type=str, default="Events",
		                               help="Y-axis label name. [Default: %(default)s]")
		self.axis_options.add_argument("--y-subplot-label", type=str, default="Ratio",
		                               help="Y-axis label name of a possible subplot. [Default: %(default)s]")
		self.axis_options.add_argument("--y-log", action="store_true", default=False,
		                               help="Logarithmic y-axis.")
		self.axis_options.add_argument("--y-ticks", type=float, nargs="+",
		                               help="Custom ticks for the Y-axis")
		self.axis_options.add_argument("--y-tick-labels", type=str, nargs="+",
		                               help="Custom tick labels for the Y-axis")
		
		self.axis_options.add_argument("--z-lims", type=float, nargs=2,
		                               help="Lower and Upper limit for z-axis.")
		self.axis_options.add_argument("--z-label", type=str, default="Events",
		                               help="Z-axis label name. [Default: %(default)s]")
		self.axis_options.add_argument("--z-log", action="store_true", default=False,
		                               help="Logarithmic z-axis.")
		self.axis_options.add_argument("--z-ticks", type=float, nargs="+",
		                               help="Custom ticks for the Z-axis")
		self.axis_options.add_argument("--z-tick-labels", type=str, nargs="+",
		                               help="Custom tick labels for the Z-axis")
		# self.axis_options.add_argument("--axes-layout", type=int, nargs=2, default=[1,1],
		                                     # help="Number of axis/pad element(s) in xy-direction. Default is %(default)s")
		# self.axis_options.add_argument("--axes", type=int, nargs="+", default=0,
		                                     # help="Index/Indices of axis/pad element(s) on which a plot is plotted. Default is %(default)s")

		#plot formatting
		self.formatting_options = parser.add_argument_group("Formatting options")
		self.formatting_options.add_argument("-C", "--colors", type=str, nargs="+",
		                                     help="Colors for the plots.")
		self.formatting_options.add_argument("-L", "--labels", type=str, nargs="+",
		                                     help="Labels for the plots.")
		self.formatting_options.add_argument("-m", "--markers", type=str, nargs="+",
		                                     help="Style for the plots.")
		self.formatting_options.add_argument("--line-styles", nargs="+",
                                             help="Line style of plots line. [Default: %(default)s]")
		self.formatting_options.add_argument("--x-errors", type='bool', nargs="+",
		                                     help="Show x errors for the nicks. [Default: True for first plot, False otherwise]")
		self.formatting_options.add_argument("--y-errors", type='bool', nargs="+",
		                                     help="Show y errors for the plots. [Default: True for first plot, False otherwise]")
		self.formatting_options.add_argument("--legend", type=str, nargs="?",
		                                     help="Location of the legend. Use 'None' to not set any legend")
		self.formatting_options.add_argument("-G", "--grid", action="store_true", default=False,
		                                     help="Place an axes grid on the plot.")
		self.formatting_options.add_argument("--subplot-grid", action="store_true", default=False,
		                                     help="Place an axes grid on the subplot.")
		self.formatting_options.add_argument("--stacks", type=str, nargs="+", default=[None],
		                                     help="Defines nick names for stacking. Inputs with the same nick name will be stacked. By default, every input gets a unique nick name. [Default: %(default)s]")

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
		self.formatting_options.add_argument("--texts", type=str, nargs="+", default=[None],
		                                     help="Place text(s) on the plot.")
		self.formatting_options.add_argument("--texts-y", type=float, nargs="+", default=[0.8],
		                                     help="Y-coordinate(s) of text(s) on plot. Ranges from 0 to 1. [Default: %(default)s]")
		self.formatting_options.add_argument("--texts-x", type=float, nargs="+", default=[0.5],
		                                     help="X-coordinate(s) of text(s) on plot. Ranges from 0 to 1. [Default: %(default)s]")
		
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
		self.other_options.add_argument("--live", default=None, nargs='?', const='gthumb',
		                                 help="Open plot in viewer after its creation. Parameter is the name of your desired viewer.")
		self.other_options.add_argument("--userpc", default=False, action='store_true',
		                                 help="If 'live' is enabled, the image will be copied to the user desktop (via ssh) and the image viewer will be started on the user desktop with this option.")
		self.other_options.add_argument("--dict", action="store_true", default=False,
		                                 help="Print out plot dictionary when plotting.")
		self.other_options.add_argument("--www", type=str, default=None, nargs='?', const="",
		                                 help="""Push output plots directly to your public EKP webspace.
		                                 Default location is http://www-ekp.physik.uni-karlsruhe.de/~<USER>/plots_archive/<DATE>
		                                 Optional argument is the name of a subdirectory that will appended to the default location.""")
	
	def prepare_args(self, parser, plotData):
		super(PlotBase, self).prepare_args(parser, plotData)
		
		# delete nicks that do not need to be used for plotting
		self.select_histograms(plotData)
		
		# prepare nick names for ratio subplot
		if plotData.plotdict["ratio"]:
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

		
		# construct labels from x/y/z expressions if not specified by user
		for labelKey, expressionKey in zip(["x_label", "y_label", "z_label"],
		                                   ["x_expressions", "y_expressions", "z_expressions"]):
			if plotData.plotdict[labelKey] == None:
				if plotData.plotdict["input_module"] == "InputInteractive":
					plotData.plotdict[labelKey] = str(labelKey[1])
				else:
					plotData.plotdict[labelKey] = reduce(lambda a, b: "%s, %s" % (str(a), str(b)), set(plotData.plotdict[expressionKey]))
			if plotData.plotdict[labelKey] == None: plotData.plotdict[labelKey] = ""
			# if y/z-expressions exist, we dont want the axis to be labelled "Events"
			if (list(set(plotData.plotdict[expressionKey])) != [None]
						and len(list(set(plotData.plotdict[expressionKey]))) == 1
						and plotData.plotdict[labelKey] == "Events"):
				plotData.plotdict[labelKey] = list(set(plotData.plotdict[expressionKey]))[0]

		# formatting options
		if plotData.plotdict["labels"] == None or all([i == None for i in plotData.plotdict["labels"]]):
			plotData.plotdict["labels"] = plotData.plotdict["nicks"]
		self.prepare_list_args(plotData, ["nicks", "colors", "labels", "markers", "line_styles", "x_errors", "y_errors"],
				n_items = max([len(plotData.plotdict[l]) for l in ["nicks"] if plotData.plotdict[l] is not None]))
		# stacks are expanded by appending None's
		plotData.plotdict["stacks"] = plotData.plotdict["stacks"]+[None]*(len(plotData.plotdict["nicks"])-len(plotData.plotdict["stacks"]))
		
		for index, error in enumerate(plotData.plotdict["x_errors"]):
			if error is None:
				plotData.plotdict["x_errors"][index] = True if index == 0 else False
		for index, error in enumerate(plotData.plotdict["y_errors"]):
			if error is None:
				plotData.plotdict["y_errors"][index] = True if index == 0 else False
		if plotData.plotdict["www"] != None:
			plotData.plotdict["output_dir"] = "/".join(['websync', datetime.date.today().strftime('%Y_%m_%d'), (plotData.plotdict["www"] or "")])
		# create output directory if not exisiting
		if not os.path.exists(plotData.plotdict["output_dir"]):
			os.makedirs(plotData.plotdict["output_dir"])
			log.info("Created output directory \"%s\"." % plotData.plotdict["output_dir"])
		
		# construct file name from x/y/z expressions if not specified by user
		if plotData.plotdict["filename"] == None:
			filename = ""
			if plotData.plotdict["input_module"] == "InputInteractive":
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
			self.prepare_list_args(plotData, ["texts", "texts_y", "texts_x"])

	def run(self, plotData):
		super(PlotBase, self).run(plotData)
		
		self.set_style(plotData)
		self.calculate_ratios(plotData)
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
		
		# handle subplot regexps
		plotData.plotdict["subplot_nicks"] = tools.matching_sublist(
				plotData.plotdict["nicks"],
				plotData.plotdict["subplot_nicks"]
		)
		log.debug("Object nicks for the subplot: %s" % ", ".join(plotData.plotdict["subplot_nicks"]))
		plotData.plotdict["subplots"] = [nick in plotData.plotdict["subplot_nicks"] for nick in plotData.plotdict["nicks"]]

	def set_style(self, plotData):
		pass
		
	def calculate_ratios(self, plotData): ## todo: define ratio for functions
		if (plotData.plotdict["ratio"] == True):
			for numerator_nicks, denominator_nicks in zip(plotData.plotdict["ratio_num"],
			                                              plotData.plotdict["ratio_denom"]):
				name = hashlib.md5("_".join(numerator_nicks+denominator_nicks)).hexdigest()
				numerator_histogram = roottools.RootTools.add_root_histograms(*[plotData.plotdict["root_objects"][nick] for nick in numerator_nicks],name=name+"_numerator")
				denominator_histogram = roottools.RootTools.add_root_histograms(*[plotData.plotdict["root_objects"][nick] for nick in denominator_nicks],name=name+"_denominator")
				
				if all([isinstance(h, ROOT.TProfile) for h in [numerator_histogram, denominator_histogram]]):
					# convert TProfiles to TH1 because ROOT can't divide TProfils correctly
					# https://root.cern.ch/phpBB3/viewtopic.php?f=3&t=2101
					# TODO is there a better way to do this?
					denom_th1 = denominator_histogram.ProjectionX()
					ratio_histogram = numerator_histogram.ProjectionX()
					ratio_histogram.Divide(denom_th1)
				elif all([isinstance(h, ROOT.TGraphAsymmErrors) for h in [numerator_histogram, denominator_histogram]]):
					ratio_histogram = roottools.RootTools.to_histogram(numerator_histogram.Clone(name + "_ratio"))
					ratio_histogram.Divide(roottools.RootTools.to_histogram(denominator_histogram))
				else:
					ratio_histogram = numerator_histogram.Clone(name + "_ratio")
					ratio_histogram.Divide(denominator_histogram)
				plotData.plotdict.setdefault("root_ratio_histos", []).append(ratio_histogram)
	
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
			plot_x_min, plot_x_max, plot_y_min, plot_y_max, plot_z_min, plot_z_max, max_dim = PlotBase.get_plot_lims(plotData.plotdict["root_objects"][nick])
			
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
		pass

	def plot_end(self, plotData):
		if not (plotData.plotdict["live"]==None):
			# if 'userpc', get the username and name of desktop machine
			if plotData.plotdict["userpc"]:
				userpc = ("%s@%s" % (tools.get_environment_variable("USER"), tools.get_environment_variable("HARRY_USERPC"))).replace("\n", "")

			for output_filename in plotData.plotdict["output_filenames"]:
				if plotData.plotdict["userpc"]:
					extrafunctions.show_plot_userpc(output_filename, plotData.plotdict["live"], tools.get_environment_variable('USER'), userpc)
				else:
					extrafunctions.show_plot(output_filename, plotData.plotdict["live"])

		# web plotting
		# TODO: make this more configurable if users want to user other webspaces etc.
		if plotData.plotdict["www"] != None:
			# set some needed variables
			user = tools.get_environment_variable("HARRY_REMOTE_USER")
			overview_filename = 'overview.html'
			date = datetime.date.today().strftime('%Y_%m_%d')
			remote_dir = tools.get_environment_variable('HARRY_REMOTE_DIR')+'/%s/%s/' % (date, (plotData.plotdict["www"] if type(plotData.plotdict["www"])==str else ""))
			remote_path = tools.get_environment_variable('HARRY_REMOTE_PATH') + '/%s' % remote_dir
			url = tools.get_environment_variable('HARRY_URL') + "/%s/%s" % (remote_dir, overview_filename)
			plots = sorted(os.listdir(plotData.plotdict["output_dir"]))
			content = ""
			n_plots_copied = 0

			log.info("Copying plots to webspace...")
			# loop over plots, make gallery
			for plot in [p for p in plots if (('.png' in p) or ('.pdf' in p))]:
				log.debug("File %s will be copied" % plot)
				# try to link to pdf file, if it exists
				href = plot.replace('.png', '.pdf')
				if href not in plots:
					href = plot
				title = plot.split('/')[-1][:-4].replace('_', ' ')
				content += htmlTemplatePlot % (title, href, title, plot)
				n_plots_copied += 1
			with open(plotData.plotdict["output_dir"] + '/' + overview_filename, 'w') as f:
				f.write(htmlTemplate % (url, content))
			if os.path.basename(url) not in plots:
				plots.append(os.path.basename(url))

			# create remote dir, copy plots and overview file
			create_dir_command = ['ssh', tools.get_environment_variable('HARRY_SSHPC'), 'mkdir -p', remote_path]
			log.debug("\nIssueing mkdir command: " + " ".join(create_dir_command))
			subprocess.call(create_dir_command)
			rsync_command =['rsync', '-u'] + [os.path.join(plotData.plotdict["output_dir"], p) for p in plots] + ["%s:%s" % (tools.get_environment_variable('HARRY_SSHPC'), remote_path)]
			log.debug("\nIssueing rsync command: " + " ".join(rsync_command) + "\n")
			subprocess.call(rsync_command)
			log.info("Copied %d plot(s) to %s" % (n_plots_copied, url))

		if plotData.plotdict["dict"]:
			pprint.pprint(plotData.plotdict)
	
	@staticmethod
	def get_plot_lims(root_object):
		x_min = None
		x_max = None
		y_min = None
		y_max = None
		z_min = None
		z_max = None
		max_dim = 2
		if isinstance(root_object, ROOT.TH1):
			x_min = root_object.GetXaxis().GetXmin()
			x_max = root_object.GetXaxis().GetXmax()
			if root_object.GetDimension() == 1:
				y_min = root_object.GetMinimum()
				y_max = root_object.GetMaximum()
			else:
				max_dim = 3
				y_min = root_object.GetYaxis().GetXmin()
				y_max = root_object.GetYaxis().GetXmax()
				if root_object.GetDimension() == 2:
					z_min = root_object.GetMinimum()
					z_max = root_object.GetMaximum()
				else:
					z_min = root_object.GetZaxis().GetXmin()
					z_max = root_object.GetZaxis().GetXmax()
		elif isinstance(root_object, ROOT.TGraph):
			# TODO: treat error bars
			x_min = root_object.GetXaxis().GetXmin()
			x_max = root_object.GetXaxis().GetXmax()
			if not isinstance(root_object, ROOT.TGraph2D):
				y_min = root_object.GetMinimum()
				y_max = root_object.GetMaximum()
			else:
				max_dim = 3
				y_min = root_object.GetYaxis().GetXmin()
				y_max = root_object.GetYaxis().GetXmax()
				z_min = root_object.GetMinimum()
				z_max = root_object.GetMaximum()
		else:
			log.warning("Retrieving the plot limits is not yet implemented for objects of type %s!." % str(type(root_object)))
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


# these html templates are needed to create the web galleries
htmlTemplate = """<!DOCTYPE html>
<html>
<head>
<style type="text/css">
div { float:left; }
pre { display: inline; padding: 3px 7px; font-size: 16px; background-color: #F5F5F5; border: 1px solid rgba(0, 0, 0, 0.15); border-radius: 4px; }
h3 { color: #888; font-size: 16px; }
</style>
</head>
<body>
<h1>Plot overview</h1>
<p>A <a href=".">file list</a> is also available and all plots can be downloaded using</p>
<p><code>wget -r -l 1 %s</code></p>
%s
</body>
</html>
"""
htmlTemplatePlot = """<div><h3>%s</h3><a href="%s" title="%s"><img src="%s" height="400"></a></div>\n"""
