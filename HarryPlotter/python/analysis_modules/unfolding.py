# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT
import copy

import Artus.HarryPlotter.analysisbase as analysisbase


class Unfolding(analysisbase.AnalysisBase):
	"""This module implements an unfolding procedure based on RooUnfold.
	
	Needed are
        - The histogram to be unfolded (nick)
        - MC gen-level histogram
        - MC reco-level histogram
        - 2D response matrix
        - path to libRooUnfold library

        optional: a variation parameter (default: 0) to vary the response matrix
            by e.g. one sigma up or down (variation = +1 or -1)

        see http://hepunx.rl.ac.uk/~adye/software/unfold/RooUnfold.html

        Example config = {
            'analysis_modules': ['Unfolding'],
            # configure input and output nicks:
            'unfolding': ['nick_data'],
            'unfolding_mc_gen': 'nick_mc_gen',
            'unfolding_mc_reco': 'mc_reco',
            'unfolding_new_nicks': ['data_unfolded'],
            'unfolding_responsematrix': 'nick_responsematrix',  # 2D histo, others are 1D
            # method and lib settings
            'unfolding_method': 'dagostini',
            'unfolding_iterations': 4,
            'libRooUnfold': '~/home/RooUnfold/libRooUnfold.so',
	    #Settings for plotting 1D and 2D in one histo:
	    'y_expressions' : [None, None,None,'genzpt'],
	    'x_expressions': ['zpt', 'zpt', 'genzpt', 'zpt'],
	    'y_bins' : [None, None, None, '30 40 50 60 80 100 120 140 170 200 1000'],
        }
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
				choices=['dagostini', 'svd', 'binbybin', 'inversion'],
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
			unfolded_histo = self.do_unfolding(
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


	def do_unfolding(self, hResponse_input, hMeas, hTrue, hData, method, libRooUnfold_path, variation=0, iterations=4):
		"""return unfolded distribution"""
		ROOT.gSystem.Load(libRooUnfold_path)

		hResponse = copy.deepcopy(hResponse_input)

		# variation (+/- one sigma of response matrix entries)
		if variation is not 0:
			for x in range(hResponse.GetNbinsX()):
				for y in range(hResponse.GetNbinsY()):
					hResponse.SetBinContent(x, y, hResponse.GetBinContent(x, y) + variation * hResponse.GetBinError(x, y))

		response = ROOT.RooUnfoldResponse(hMeas, hTrue, hResponse, hMeas.GetName(), hMeas.GetTitle())

		unfolding_methods = {
			'dagostini': ROOT.RooUnfoldBayes,  # iterative dagostini
			'svd': ROOT.RooUnfoldSvd,  # singular value decomposition
			'binbybin': ROOT.RooUnfoldBinByBin,  # bin-by-bin (simple correction factors)
			'inversion': ROOT.RooUnfoldInvert,  # inversion
		}

		if method in ['dagostini', 'svd']:
			unfold = unfolding_methods[method](response, hData, iterations)
		else:
			unfold = unfolding_methods[method](response, hData)
		return unfold.Hreco()
