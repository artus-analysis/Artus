# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class CorrectNegativeBinContents(analysisbase.AnalysisBase):

	def __init__(self):
		super(CorrectNegativeBinContents, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(CorrectNegativeBinContents, self).modify_argument_parser(parser, args)

		self.correctnegbincontent_options = parser.add_argument_group("CorrectNegativeBinContents options")
		self.correctnegbincontent_options.add_argument("--nicks-correct-negative-bins", nargs="+",
							       default=[],
							       help="Nicks of histograms to be corrected. [Default: %(default)s]")
	
	def prepare_args(self, parser, plotData):
		super(CorrectNegativeBinContents, self).prepare_args(parser, plotData)

		plotData.plotdict["nicks_correct_negative_bins"] = [nicks.split() for nicks in plotData.plotdict["nicks_correct_negative_bins"]]
		if len(plotData.plotdict["nicks_correct_negative_bins"]) == 0:
			log.warning("CorrectNegativeBinContents module called, but no nicks of the histograms to be corrected are provided")
	
	def run(self, plotData=None):
		super(CorrectNegativeBinContents, self).run(plotData)
		
		# loop over all histograms
		for nicks_correct_negative_bins in plotData.plotdict["nicks_correct_negative_bins"]:
			for nick in nicks_correct_negative_bins:
				root_histogram = plotData.plotdict["root_objects"][nick]
				if isinstance(root_histogram, ROOT.TH1):
					original_integral = root_histogram.Integral()
					negative_bin_contents_found = False
				
					# loop over all bins
					for x_bin in xrange(1, root_histogram.GetNbinsX()+1):
						for y_bin in xrange(1, root_histogram.GetNbinsY()+1):
							for z_bin in xrange(1, root_histogram.GetNbinsY()+1):
								global_bin = root_histogram.GetBin(x_bin, y_bin, z_bin)
							
								# set negative bin contents to zero
								if root_histogram.GetBinContent(global_bin) < 0.0:
									root_histogram.SetBinContent(global_bin, 0.0)
									negative_bin_contents_found = True
				
					# preserve original integral
					if negative_bin_contents_found:
						root_histogram.Scale(abs(original_integral / root_histogram.Integral()))


