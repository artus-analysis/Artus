# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import abc
import os
import re

import Artus.HarryPlotter.processor as processor


class PlotBase(processor.Processor):
	
	def __init__(self):
		processor.Processor.__init__(self)
	
	def modify_argument_parser(self, parser):
		processor.Processor.modify_argument_parser(self, parser)
		
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
		#self.axis_options.add_argument("--xview", type=float, nargs=2, default=xview,
		#                               help="Lower and Upper limit for x-axis viewing in the plot")
		
		self.axis_options.add_argument("--y-lims", type=float, nargs=2,
		                               help="Lower and Upper limit for y-axis.")
		self.axis_options.add_argument("--y-ratio-lims", type=float, nargs=2,
		                               help="Lower and Upper limit for y-axis of a possible ratio subplot.")
		self.axis_options.add_argument("--y-label", type=str, default="Events",
		                               help="Y-axis label name. [Default: %(default)s]")
		self.axis_options.add_argument("--y-log", action="store_true", default=False,
		                               help="Logarithmic y-axis.")
		self.axis_options.add_argument("--y-ticks", type=float, nargs="+",
		                               help="Custom ticks for the Y-axis")
		
		self.axis_options.add_argument("--z-lims", type=float, nargs=2,
		                               help="Lower and Upper limit for z-axis.")
		self.axis_options.add_argument("--z-label", type=str, default="Events",
		                               help="Z-axis label name. [Default: %(default)s]")
		self.axis_options.add_argument("--z-log", action="store_true", default=False,
		                               help="Logarithmic z-axis.")
		self.axis_options.add_argument("--z-ticks", type=float, nargs="+",
		                               help="Custom ticks for the Z-axis")

		# plot labelling
		self.labelling_options = parser.add_argument_group("Labelling options")
		self.labelling_options.add_argument("-t", "--title", type=str,
		                                    help="Plot title")
		self.labelling_options.add_argument("-l", "--lumi", type=float,
		                                    help="Luminosity for the given data in /fb.")
		self.labelling_options.add_argument("-e", "--energy", type=int,
		                                    help="Centre-of-mass energy for the given samples in TeV.")
		self.labelling_options.add_argument("-A", "--author", type=str,
		                                    help="author name of the plot")
		self.labelling_options.add_argument("--date", type=str,
		                                    help="Show the date in the top left corner. \"iso\" is YYYY-MM-DD, \"today\" is DD Mon YYYY and \"now\" is DD Mon YYYY HH:MM.")
		self.labelling_options.add_argument("-E", "--event-number-label", action="store_true", default=False,
		                                    help="Add event number label")
		
		# output settings
		self.output_options = parser.add_argument_group("Output options")
		self.output_options.add_argument("-o", "--output-dir", default="plots",
		                                 help="Output directory for plots. [Default: %(default)s]")
		self.output_options.add_argument("--filename",
		                                 help="Filename of the plot without extension. By default, it is constructed from the x/y/z expressions.")
		self.output_options.add_argument("-f", "--formats", nargs="+", default=["png"],
		                                 help="Format of the plots. [Default: %(default)s]")
	
	def prepare_args(self, plotData):
		processor.Processor.prepare_args(self, plotData)
		
		# construct labels from x/y/z expressions if not specified by user
		for labelKey, expressionKey in zip(["x_label", "y_label", "z_label"],
		                                   ["x_expressions", "y_expressions", "z_expressions"]):
			if plotData.plotdict[labelKey] == None:
				plotData.plotdict[labelKey] = reduce(lambda a, b: "%s, %s" % (str(a), str(b)), set(plotData.plotdict[expressionKey]))
			if plotData.plotdict[labelKey] == None: plotData.plotdict[labelKey] = ""
		
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
	
	def run(self, plotData):
		processor.Processor.run(self, plotData)
		
		self.create_canvas(plotData)
		self.prepare_histograms(plotData)
		self.make_plots(plotData)
		self.modify_axes(plotData)
		self.add_labels(plotData)
		self.add_texts(plotData)
		self.save_canvas(plotData)
	
	@abc.abstractmethod
	def create_canvas(self, plotData):
		pass
	
	@abc.abstractmethod
	def prepare_histograms(self, plotData):
		pass
	
	@abc.abstractmethod
	def make_plots(self, plotData):
		pass
	
	@abc.abstractmethod
	def modify_axes(self, plotData):
		pass
	
	@abc.abstractmethod
	def add_labels(self, plotData):
		pass
	
	@abc.abstractmethod
	def add_texts(self, plotData):
		pass
	
	@abc.abstractmethod
	def save_canvas(self, plotData):
		pass

