# -*- coding: utf-8 -*-

"""
	Creates ROOT histograms by evaluating fastNLO tables.
	Usage: 	--input-modules InputFastNLO --files fastnlo_table.txt
			[--members 1 2 3 ... --pdf-sets NNPDF.LHgrid ...]
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

from fastnlo import fastNLOLHAPDF
import ROOT
import numpy as np
from array import array

import Artus.HarryPlotter.inputbase as inputbase


class InputFastNLO(inputbase.InputBase):

	def modify_argument_parser(self, parser, args):
		self.fastnlo_options = parser.add_argument_group("fastNLO input options")
		self.fastnlo_options.add_argument("--fastnlo-files", type=str, nargs="+",
			help="Input fastnlo table(s).")
		self.fastnlo_options.add_argument('--pdf-sets', type=str, nargs="+", default=['CT10nlo.LHgrid'],
			help="PDF sets. [Default: %(default)s]")
		self.fastnlo_options.add_argument('--members', type=int, nargs="+", default=[0],
			help="PDF set members. [Default: %(default)s]")


	def prepare_args(self, parser, plotData):
		if isinstance(plotData.plotdict['fastnlo_files'], basestring):
			plotData.plotdict['fastnlo_files'] = [plotData.plotdict['fastnlo_files']]
		self.prepare_list_args(plotData, ["fastnlo_files", "pdf_sets", "members"])


	def run(self, plotData):
		for filename, pdfset, member in zip(
				plotData.plotdict['fastnlo_files'],
				plotData.plotdict['pdf_sets'],
				plotData.plotdict['members']
		):
			fnlo = fastNLOLHAPDF(str(filename))
			fnlo.SetLHAPDFFilename(str(pdfset))
			fnlo.SetLHAPDFMember(member)
			fnlo.CalcCrossSection()

			# create histogram
			x_binning = sorted(list(set([item for sublist in fnlo.GetDim0BinBoundaries() for item in sublist])))
			root_histogram = ROOT.TH1D(str(member),str(member),len(x_binning)-1, array('d', x_binning))

			# fill values for central xsec
			xs = np.array(fnlo.GetCrossSection())
			xs[xs <= 0.] = 0.  # ?
			for i in range(0, fnlo.GetNDim0Bins()):
				root_histogram.SetBinContent(i+1, xs[i])

			# append nick and histo to plotdict
			nick = "_".join([filename, pdfset, str(member)])
			plotData.plotdict.setdefault("nicks", []).append(nick)
			plotData.plotdict.setdefault("root_objects", {})[nick] = root_histogram
