# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)
import sys
import ROOT
import Artus.HarryPlotter.analysisbase as analysisbase
import copy

class DecayModes(analysisbase.AnalysisBase):
	def __init__(self):
		super(DecayModes, self).__init__()

	def modify_argument_parser(self, parser, args):
		pass
		"""
		self.projectbyfit_options = parser.add_argument_group("Options for ProjectByFit module. See TH2::FitSlicesY for documentation.")
		self.projectbyfit_options.add_argument("--projection-function", type=str, nargs="+", default=None,
								help="Function to project along Y-Axis")
		self.projectbyfit_options.add_argument("--projection-parameters", type=str, nargs="+", default=None,
								help="Starting parameters for the fit, comma seperated.")
		self.projectbyfit_options.add_argument("--projection-to-plot", type=str, nargs="+", default="0",
								help="Parameter number that should be plotted or TFormula to combine parameters of fit.")
		self.projectbyfit_options.add_argument("--projection-fit-range", type=str, nargs="+", default=None,
								help="Y-Range to perform the fit in the format \"low,high\". Default is the whole range of the input 2D Histogram.")
		self.projectbyfit_options.add_argument("--projection-to-nick", type=str, nargs="+", default="nick0",
						help="Nickname of 2D input histogram. Default: nick0")
		self.projectbyfit_options.add_argument("--fit-backend", type=str, nargs="+", default="ROOT",
								help="Fit backend. ROOT and RooFit are available. Check sourcecode which parts of RooFit are implemented. [Default: %(default)s]")
		"""
	def prepare_args(self, parser, plotData):
		pass#super(DecayModes, self).prepare_args(parser, plotData)

	def run(self, plotData=None):
		#super(DecayModes, self).run()
		generator_dms = plotData.plotdict["x_expressions"]
		print generator_dms
		reco_dms = plotData.plotdict["folders"][0]
		print reco_dms
		histogram = ROOT.TH2F("histogram", "Decay Channel", len(generator_dms)+1, 0, len(generator_dms)+1, len(reco_dms)+1, 0, len(reco_dms)+1)

		for index_reco, (nick, tree) in enumerate(plotData.plotdict.get("root_trees", {}).items()):
			inversion_string = "(1)"
			for index_gen, gen_dm in enumerate(generator_dms):
				selection_string = gen_dm + ">0.5"
				inversion_string += "&&("+gen_dm + "<0.5)"
				tmp = tree.Draw("1>>histoname", selection_string)
				root_histo = ROOT.gDirectory.Get("histoname")
				histogram.SetBinContent(index_gen+1, index_reco+1, root_histo.Integral())
			tmp = tree.Draw("1>>histoname", inversion_string)
			root_histo = ROOT.gDirectory.Get("histoname")
			histogram.SetBinContent(index_gen+1, index_reco+1, root_histo.Integral())

		x_labels = generator_dms + ["none"]
		y_labels = reco_dms + ["none"]
		for i, label in enumerate(x_labels):
			histogram.GetXaxis().SetBinLabel(i+1, label)
		for i, label in enumerate(y_labels):
			histogram.GetYaxis().SetBinLabel(i+1, label)
		plotData.plotdict["nicks"] = ['2dhisto']
		plotData.plotdict["root_objects"]["2dhisto"] = histogram

