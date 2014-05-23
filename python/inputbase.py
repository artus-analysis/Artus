# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import abc
import collections

import Artus.HarryPlotter.processor as processor


class InputBase(processor.Processor):
	def __init__(self):
		processor.Processor.__init__(self)
	
	def modify_argument_parser(self, parser):
		self.input_options = parser.add_argument_group("Input options")
		self.input_options.add_argument("-i", "--files", type=str, nargs="+",
		                                 help="Input root file(s).")
		self.input_options.add_argument("--nicks", type=str, nargs="+",
		                                 help="Defines nick names for inputs. Inputs with the same nick name will be merged. By default, every input gets a unique nick name.")

		self.input_options.add_argument("-x", "--x-expressions", type=str, nargs="+",
		                                help="x-axis variable expression(s)")
		self.input_options.add_argument("-y", "--y-expressions", type=str, nargs="+",
		                                help="y-axis variable expression(s)")
		self.input_options.add_argument("-z", "--z-expressions", type=str, nargs="+",
		                                help="z-axis variable expression(s)")
		self.input_options.add_argument("-w", "--weights", type=str, nargs="+", default="1.0",
		                                help="Weight (cut) expression(s). [Default: %(default)s]")

		self.input_options.add_argument("--type", type=str, # TODO: default
		                                help="Type of plot.")
		self.input_options.add_argument("-n", "--norm-references", type=str, nargs="+",
		                                help="Nick names of inputs or float values to normalise to. One parameter can contain a whitespace separated list of multiple nick names, that are summed up before the normalisation.")
	
	def prepare_args(self, plotData):
		processor.Processor.prepare_args(self, plotData)
		
		self.prepare_list_args(plotData, ["files", "nicks", "x_expressions", "y_expressions",
		                                  "z_expressions", "weights", "norm_references"])
		plotData.plotdict["files"] = [files.split() for files in plotData.plotdict["files"]]
		plotData.plotdict["nicks"] = [nick if nick != None else ("nick%d" % index) for index, nick in enumerate(plotData.plotdict["nicks"])]
	
	def run(self, plotData):
		processor.Processor.run(self, plotData)

	@abc.abstractmethod
	def read_root_objects(self):
		return

