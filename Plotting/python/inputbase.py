# -*- coding: utf-8 -*-

"""
"""

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import HarryPlotter.Plotting.processor as processor


class InputBase(processor.Processor):
	def __init__(self):
		super(InputBase, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		self.input_options = parser.add_argument_group("Input options")
		self.input_options.add_argument("--nicks", type=str, nargs="+",
		                                 help="Defines nick names for inputs. Inputs with the same nick name will be merged. By default, every input gets a unique nick name.")
		self.input_options.add_argument("-x", "--x-expressions", type=str, nargs="+",
		                                help="x-value(s)")
		self.input_options.add_argument("-y", "--y-expressions", type=str, nargs="+",
		                                help="y-value(s)")
		self.input_options.add_argument("-z", "--z-expressions", type=str, nargs="+",
		                                help="z-value(s)")
	
	def prepare_args(self, parser, plotData):
		super(InputBase, self).prepare_args(parser, plotData)
		
		self.prepare_list_args(plotData, ["nicks", "x_expressions", "y_expressions", "z_expressions"])
		
		# prepare nicks
		plotData.plotdict["nicks"] = [nick if nick != None else ("nick%d" % index) for index, nick in enumerate(plotData.plotdict["nicks"])]
	
	def run(self, plotData):
		super(InputBase, self).run(plotData)

