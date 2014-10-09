# -*- coding: utf-8 -*-

"""
"""

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import HarryPlotter.Plotting.analysisbase as analysisbase


class ScaleBinContentsByWidth(analysisbase.AnalysisBase):
	def __init__(self):
		super(ScaleBinContentsByWidth, self).__init__()
		
	
	def run(self, plotData=None):
		super(ScaleBinContentsByWidth, self).run(plotData)
		
		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			if isinstance(root_histogram, ROOT.TH1):
			
				for x_bin in xrange(1, root_histogram.GetNbinsX()+1):
					for y_bin in xrange(1, root_histogram.GetNbinsY()+1):
						for z_bin in xrange(1, root_histogram.GetNbinsY()+1):
							global_bin = root_histogram.GetBin(x_bin, y_bin, z_bin)
							
							bin_width = root_histogram.GetXaxis().GetBinWidth(x_bin) * \
							            (1.0 if root_histogram.GetNbinsY() == 1 else root_histogram.GetYaxis().GetBinWidth(y_bin)) * \
							            (1.0 if root_histogram.GetNbinsZ() == 1 else root_histogram.GetZaxis().GetBinWidth(z_bin))
							
							root_histogram.SetBinContent(global_bin, root_histogram.GetBinContent(global_bin) / bin_width)
							root_histogram.SetBinError(global_bin, root_histogram.GetBinError(global_bin) / bin_width)
							# sum of weights^2 might not treated correctly, therefore use this always as the last analysis module

