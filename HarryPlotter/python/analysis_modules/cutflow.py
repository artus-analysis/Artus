# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase
import Artus.HarryPlotter.utility.extrafunctions as extrafunctions


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

		# get list of filternames (= bin labels) from all cutflow histograms
		filternames = {}
		for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
			filternames[nick] = []
			for binnumber in range(1, root_histogram.GetNbinsX()+1):
				filternames[nick].append(root_histogram.GetXaxis().GetBinLabel(binnumber))

		# check whether number of bins is the same for all cutflow histograms:
		if len(set( [len(filternames[nick]) for nick in filternames] )) > 1:
			log.warning("Cutflow histograms have different number of bins! New histograms containing all cuts will be constructed.")

			# construct list containing ALL cuts while preserve ordering
			allcuts = filternames[plotData.plotdict["root_objects"].keys()[0]]
			for nick in plotData.plotdict["root_objects"].keys()[1:]:
				allcuts = extrafunctions.merge_sequences(allcuts, filternames[nick])

			for nick, root_histogram in plotData.plotdict["root_objects"].iteritems():
				# new histo with bins for all cuts
				new_histo = ROOT.TH1F(root_histogram.GetName()+"_new", root_histogram.GetTitle()+"_new", len(allcuts), 0, len(allcuts))
				cut_efficiency = 1.
				cut_efficiency_error = 0.
				for index, cut in enumerate(allcuts):
					# if the cut is available for the current nick, use its efficiceny.
					# if not, take the efficiency from the last available cut.
					if cut in filternames[nick]:
						cut_efficiency = root_histogram.GetBinContent(filternames[nick].index(cut)+1)
						cut_efficiency_error = root_histogram.GetBinError(filternames[nick].index(cut)+1)
					elif plotData.plotdict["rel_cuts"]:
						cut_efficiency = 1.
					new_histo.SetBinContent(index+1, cut_efficiency)
					new_histo.SetBinError(index+1, cut_efficiency_error)
					new_histo.GetXaxis().SetBinLabel(index+1, cut)
				plotData.plotdict["root_objects"][nick] = new_histo
