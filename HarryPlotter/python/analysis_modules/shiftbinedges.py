# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class ShiftBinEdges(analysisbase.AnalysisBase):
	"""Move bin edges of histograms by constant shift."""

	def modify_argument_parser(self, parser, args):
		super(ShiftBinEdges, self).modify_argument_parser(parser, args)

		self.add_histograms_options = parser.add_argument_group("{} options".format(self.name()))
		self.add_histograms_options.add_argument(
				"--shift-bin-edges-nicks", nargs="+",
				help="Nick names for the histograms to be treated in place"
		)
		self.add_histograms_options.add_argument(
				"--x-bins-shifts", nargs="+", type=float, default=[0.0],
				help="Shifts for bin edges in x-direction. [Default: %(default)s]"
		)
		self.add_histograms_options.add_argument(
				"--y-bins-shifts", nargs="+", type=float, default=[0.0],
				help="Shifts for bin edges in y-direction. [Default: %(default)s]"
		)
		self.add_histograms_options.add_argument(
				"--z-bins-shifts", nargs="+", type=float, default=[0.0],
				help="Shifts for bin edges in z-direction. [Default: %(default)s]"
		)

	def prepare_args(self, parser, plotData):
		super(ShiftBinEdges, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["shift_bin_edges_nicks", "x_bins_shifts", "y_bins_shifts", "z_bins_shifts"])

	def run(self, plotData=None):
		super(ShiftBinEdges, self).run(plotData)
		
		for nick, x_bins_shift, y_bins_shift, z_bins_shift in zip(
				*[plotData.plotdict[k] for k in ["shift_bin_edges_nicks", "x_bins_shifts", "y_bins_shifts", "z_bins_shifts"]]
		):
			root_histogram = plotData.plotdict["root_objects"][nick]
			assert(isinstance(root_histogram, ROOT.TH1))
			
			shifts = [x_bins_shift, y_bins_shift, z_bins_shift]
			axes = [root_histogram.GetXaxis(), root_histogram.GetYaxis(), root_histogram.GetZaxis()]
			for shift, axis in zip(shifts, axes):
				if shift != 0.0:
					old_bin_edges = [axis.GetBinLowEdge(bin_index) for bin_index in xrange(1, axis.GetNbins()+2)]
					new_bin_edges = [edge+shift for edge in old_bin_edges]
					axis.Set(axis.GetNbins(), array.array("d", new_bin_edges))
				










