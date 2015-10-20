# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT
import copy

import Artus.HarryPlotter.analysisbase as analysisbase


class Unfolding(analysisbase.AnalysisBase):
	"""This module implements a basic unfolding procedure."""
	"""Needed are
        - The histogram to be unfolded (nick)
        - MC gen-level histogram
        - MC reco-level histogram
        - 2D response matrix
        - path to libRooUnfold library

        optional: a variation parameter (default: 0) to vary the response matrix
            by e.g. one sigma up or down (variation = +1 or -1)

        see http://hepunx.rl.ac.uk/~adye/software/unfold/RooUnfold.html
	"""


	def modify_argument_parser(self, parser, args):
		super(Unfolding, self).modify_argument_parser(parser, args)

		self.Unfolding_options = parser.add_argument_group(self.name() + " options")
		self.Unfolding_options.add_argument("--unfolding", type=str, nargs='*',
				help="List the nick(s) you want to unfold.")
		self.Unfolding_options.add_argument("--unfolding-responsematrix", type=str, nargs='*',
				help="List the nick(s) you want to use as unfolding responsematrix.")
		self.Unfolding_options.add_argument("--unfolding-mc-gen", type=str, nargs='*',
				help="List the nick(s) you want to use as unfolding mc gen.")
		self.Unfolding_options.add_argument("--unfolding-mc-reco", type=str, nargs='*',
				help="List the nick(s) you want to use as unfolding mc reco.")
		self.Unfolding_options.add_argument("--unfolding-variation", type=float, nargs='*', default=[0.],
				help="List the variations")
		self.Unfolding_options.add_argument("--unfolding-new-nicks", type=str, nargs='*', default="unfolded",
				help="List the nick(s) for the unfolded histograms.")
		self.Unfolding_options.add_argument("--libRooUnfold", type=str, default="",
				help="Location of libRooUnfold library file")
		self.Unfolding_options.add_argument("--unfolding-iterations", type=int, default=4,
				help="Number of iterations for unfolding")
		self.Unfolding_options.add_argument("--unfolding-method", type=str, default="dagostini",
				choices=['dagostini', 'binbybin', 'svd'],
				help="Method for unfolding. [Default: %(default)s]")

	def prepare_args(self, parser, plotData):
		super(Unfolding, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, [
			"unfolding",
			"unfolding_responsematrix", 
			"unfolding_mc_gen",
			"unfolding_mc_reco",
			"unfolding_variation",
			"unfolding_new_nicks",
		])

	def run(self, plotData=None):
		super(Unfolding, self).run(plotData)

		for data, matrix, gen, reco, variation, new_nick in zip(
				plotData.plotdict["unfolding"],
				plotData.plotdict["unfolding_responsematrix"],
				plotData.plotdict["unfolding_mc_gen"],
				plotData.plotdict["unfolding_mc_reco"],
				plotData.plotdict["unfolding_variation"],
				plotData.plotdict["unfolding_new_nicks"],
		):
			unfolded_histo = doUnfolding(
				plotData.plotdict["root_objects"][matrix],
				plotData.plotdict["root_objects"][reco],
				plotData.plotdict["root_objects"][gen],
				plotData.plotdict["root_objects"][data],
				plotData.plotdict["unfolding_method"],
				plotData.plotdict["libRooUnfold"],
				variation,
				plotData.plotdict["unfolding_iterations"]
			)
			plotData.plotdict["root_objects"][new_nick] = unfolded_histo
			plotData.plotdict["nicks"].append(new_nick)


def doUnfolding(hResponse_input, hMeas, hTrue, hData, method, libRooUnfold_path, variation=0, iterations=4):
	"""return unfolded distribution"""
	ROOT.gSystem.Load(libRooUnfold_path)

	hResponse = copy.deepcopy(hResponse_input)

	# variation
	if variation is not 0:
		for x in range(hResponse.GetNbinsX()):
			for y in range(hResponse.GetNbinsY()):
				hResponse.SetBinContent(x, y, hResponse.GetBinContent(x, y) + variation * hResponse.GetBinError(x, y))

	response = ROOT.RooUnfoldResponse(hMeas, hTrue, hResponse, hMeas.GetName(), hMeas.GetTitle())

	#TODO make this configurable
	if method == 'dagostini':
		unfold = ROOT.RooUnfoldBayes(response, hData, iterations)
	elif method == 'svd':
		unfold = ROOT.RooUnfoldSvd(response, hData, iterations)  # singular value decomposition
	elif method == 'binbybin':
		unfold = ROOT.RooUnfoldBinByBin(response, hData)  # bin-by-bin (simple correction factors)

	hReco = unfold.Hreco()
	return hReco
