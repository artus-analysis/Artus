# -*- coding: utf-8 -*-

"""
"""

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import HarryPlotter.Plotting.analysisbase as analysisbase


class CorrectNegativeBinContents(analysisbase.AnalysisBase):

	def __init__(self):
		super(CorrectNegativeBinContents, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(CorrectNegativeBinContents, self).modify_argument_parser(parser, args)
	
	def prepare_args(self, parser, plotData):
		super(CorrectNegativeBinContents, self).prepare_args(parser, plotData)
	
	def run(self, plotData=None):
		super(CorrectNegativeBinContents, self).run(plotData)
		
		# loop over all histograms
		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
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
					root_histogram.Scale(original_integral / root_histogram.Integral()

