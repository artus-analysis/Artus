# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array
import ROOT
import sys

import Artus.HarryPlotter.inputbase as inputbase
import Artus.Utility.progressiterator as pi


class InputInteractive(inputbase.InputBase):
	def __init__(self):
		super(InputInteractive, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(InputInteractive, self).modify_argument_parser(parser, args)
		
		self.input_options.add_argument("--x-errors", type=str, nargs="+",
		                                help="x-error(s)")
		self.input_options.add_argument("--y-errors", type=str, nargs="+",
		                                help="y-error(s)")
		self.input_options.add_argument("--z-errors", type=str, nargs="+",
		                                help="z-error(s)")
	
	def prepare_args(self, parser, plotData):
		super(InputInteractive, self).prepare_args(parser, plotData)
		
		self.prepare_list_args(plotData, ["nicks", "x_expressions", "y_expressions", "z_expressions", "x_bins", "y_bins", "z_bins", "scale_factors", "x_errors", "y_errors", "z_errors"])
		
		# parse values/errors to plot
		values_keys = ["x_expressions", "y_expressions", "z_expressions"]
		errors_keys = ["x_errors", "y_errors", "z_errors"]
		for key in values_keys + errors_keys:
			if plotData.plotdict[key] != None:
				plotData.plotdict[key] = [[float(value) for value in (values.split() if values != None else [])] for values in plotData.plotdict[key]]
		
		# set default errors to 0.0
		for index in xrange(len(plotData.plotdict["nicks"])):
			for values_key, errors_key in zip(values_keys, errors_keys):
				if len(plotData.plotdict[errors_key][index]) == 0:
					plotData.plotdict[errors_key][index] = len(plotData.plotdict[values_key][index]) * [0.0]
		
		# check that x/y/z values/errors for one plot have the same size
		for index in xrange(len(plotData.plotdict["nicks"])):
			n_values_per_plot = []
			for key in values_keys + errors_keys:
				if len(plotData.plotdict[key][index]) > 0:
					n_values_per_plot.append(len(plotData.plotdict[key][index]))
			assert(len(set(n_values_per_plot)) == 1)
		
		inputbase.InputBase.prepare_nicks(plotData)
	
	def run(self, plotData):
		
		for index, (
				nick,
				x_bins, x_values, x_errors,
				y_bins, y_values, y_errors,
				z_bins, z_values, z_errors,
		) in enumerate(pi.ProgressIterator(zip(*[plotData.plotdict[key] for key in [
				"nicks",
				"x_bins", "x_expressions", "x_errors",
				"y_bins", "y_expressions", "y_errors",
				"z_bins", "z_expressions", "z_errors",
		]]), description="Reading inputs")):
			print nick, x_bins, x_values, x_errors, y_bins, y_values, y_errors, z_bins, z_values, z_errors
			
			if x_bins is None:
				if len(y_values) == 0:
					pass # TODO: create function
				else:
					if len(z_values) == 0:
						plotData.plotdict.setdefault("root_objects", {})[nick] = ROOT.TGraphErrors(
								len(x_values),
								array.array("d", x_values), array.array("d", y_values),
								array.array("d", x_errors), array.array("d", y_errors)
						)
					else:
						plotData.plotdict.setdefault("root_objects", {})[nick] = ROOT.TGraph2DErrors(
								len(x_values),
								array.array("d", x_values), array.array("d", y_values), array.array("d", z_values),
								array.array("d", x_errors), array.array("d", y_errors), array.array("d", z_errors)
						)
			elif y_bins is None:
				pass # TODO: create TH1
			elif z_bins is None:
				pass # TODO: create TH2
			else:
				pass # TODO: create TH3
		
		# run upper class function at last
		super(InputInteractive, self).run(plotData)

