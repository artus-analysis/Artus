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
	
	@staticmethod
	def expressions_to_string(expressions):
		expression_string = reduce(lambda a, b: "%s__%s" % (str(a), str(b)), set(expressions))
		expression_string = re.sub("[^a-zA-Z0-9]", "_", expression_string)
	
	def __init__(self):
		processor.Processor.__init__(self)
	
	def modify_argument_parser(self, parser):
		processor.Processor.modify_argument_parser(self, parser)
		
		self.output_options = parser.add_argument_group("Output options")
		self.output_options.add_argument("-o", "--output-dir", default="plots",
		                                 help="Output directory for plots. [Default: %(default)s]")
		self.output_options.add_argument("--filename",
		                                 help="Filename of the plot without extension. By default, it is constructed from the x/y/z expressions.")
		self.output_options.add_argument("-f", "--formats", nargs="+", default=["png"],
		                                 help="Format of the plots. [Default: %(default)s]")
	
	def prepare_args(self, plotData):
		processor.Processor.prepare_args(self, plotData)
		
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
		self.make_plots(plotData)
		self.modify_axes(plotData)
		self.add_texts(plotData)
		self.save_canvas(plotData)
	
	@abc.abstractmethod
	def create_canvas(self, plotData):
		pass
	
	@abc.abstractmethod
	def make_plots(self, plotData):
		pass
	
	@abc.abstractmethod
	def modify_axes(self, plotData):
		pass
	
	@abc.abstractmethod
	def add_texts(self, plotData):
		pass
	
	@abc.abstractmethod
	def save_canvas(self, plotData):
		pass

