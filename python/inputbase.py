# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import abc

import Artus.HarryPlotter.processor as processor


class InputBase(processor.Processor):
	def __init__(self):
		processor.Processor.__init__(self)
	
	def modify_argument_parser(self, parser):
		self.input_options = parser.add_argument_group("Input options")
		self.input_options.add_argument("-i", "--files", type=str, nargs="*",
		                                 help="Input root file(s).")
		self.input_options.add_argument("--nick", type=str, nargs="*",
		                                 help="Defines nick names for inputs. Inputs with the same nick name will be merged. By default, every input gets a unique nick name.")

		self.input_options.add_argument("-x", "-x-expressions", type=str, nargs="*",
		                                help="x-axis variable expression(s)")
		self.input_options.add_argument("-y", "-y-expressions", type=str, nargs="*",
		                                help="y-axis variable expression(s)")
		self.input_options.add_argument("-z", "-z-expressions", type=str, nargs="*",
		                                help="z-axis variable expression(s)")
		self.input_options.add_argument("-w", "--weights", type=str, nargs="+", default="1.0",
		                                help="Weight (cut) expression(s)")

		self.input_options.add_argument("--type", type=str, # TODO: default
		                                help="Type of plot.")
		self.input_options.add_argument("-n", "--normalize", type=str, nargs="*",
		                                help="Nick names of inputs or float values to normalise to. One parameter can contain a whitespace separated list of multiple nick names, that are summed up before the normalisation.")
	
	def run(self, plotdict=None):
		processor.Processor.run(self, plotdict)

	@abc.abstractmethod
	def read_root_objects(self):
		return

