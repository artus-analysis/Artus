# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.analysis_modules.histogrammanipulationbase as histogrammanipulationbase

class ShiftBinContents(histogrammanipulationbase.HistogramManipulationBase):
	"""Shift bin contents by a certain value (for selected histograms)."""

	def modify_argument_parser(self, parser, args):
		super(ShiftBinContents, self).modify_argument_parser(parser, args)

		self.ShiftBinContents_options = parser.add_argument_group("ShiftBinContents options")
		self.ShiftBinContents_options.add_argument("--shift-bin-contents", type=str, nargs='*', default=[],
				help="List the nicks of the histograms you want to shift (-> module whitelist ).")
		self.ShiftBinContents_options.add_argument("--shift", type=float, default=0.,
				help="Value by which you want the bin contents to be shifted.")

	def prepare_args(self, parser, plotData):
		super(ShiftBinContents, self).prepare_args(parser, plotData)
		self.whitelist = plotData.plotdict['shift_bin_contents']
		self.shift = plotData.plotdict['shift']

	def _manipulate_bin(self, histogram, global_bin):
		histogram.SetBinContent(global_bin, histogram.GetBinContent(global_bin) + self.shift)
