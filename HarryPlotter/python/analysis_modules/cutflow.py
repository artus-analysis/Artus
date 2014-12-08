# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class Cutflow(analysisbase.AnalysisBase):

	def __init__(self):
		super(Cutflow, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(Cutflow, self).modify_argument_parser(parser, args)
		
		self.cutflow_options = parser.add_argument_group("Cutflow options")
		self.cutflow_options.add_argument("--rel-cuts", action="store_true", default=False,
				help="Scale bins relative to respective left bins.")
	
	def prepare_args(self, parser, plotData):
		super(Cutflow, self).prepare_args(parser, plotData)
		
		if plotData.plotdict["x_label"] == parser.get_default("x_label"):
			plotData.plotdict["x_label"] = ""
		if plotData.plotdict["y_label"] == parser.get_default("y_label"):
			plotData.plotdict["y_label"] = ("Relative " if plotData.plotdict["rel_cuts"] else "") + "Cut Efficiency"
		
		if plotData.plotdict["y_lims"] == parser.get_default("y_lims"):
			plotData.plotdict["y_lims"] = [1e-5, 1.5] if plotData.plotdict["y_log"] else [0.0, 1.1]
	
	def run(self, plotData=None):
		super(Cutflow, self).run(plotData)
		
		# loop over all histograms
		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			if isinstance(root_histogram, ROOT.TH1):
				if plotData.plotdict["rel_cuts"]:
					for x_bin in xrange(root_histogram.GetNbinsX(), 1, -1):
						new_bin_content = root_histogram.GetBinContent(x_bin)
						if new_bin_content != 0:
							new_bin_content /= root_histogram.GetBinContent(x_bin - 1)
						root_histogram.SetBinContent(x_bin, new_bin_content)
					root_histogram.SetBinContent(1, 1.0)
				else:
					root_histogram.Scale(1.0 / root_histogram.GetBinContent(1))

