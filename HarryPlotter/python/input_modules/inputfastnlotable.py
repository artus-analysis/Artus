# -*- coding: utf-8 -*-

"""
	Creates ROOT histograms by evaluating fastNLO tables.
	Usage: 	--input-modules InputFastNLOTable --files fastnlo_table.txt
			[--members 1 2 3 ... --pdf-sets NNPDF.LHgrid ...]
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

from fastnlo import fastNLOLHAPDF
import ROOT
import numpy as np
from array import array

import Artus.HarryPlotter.input_modules.inputfile as inputfile


class InputFastNLOTable(inputfile.InputFile):

	def modify_argument_parser(self, parser, args):
		super(InputFastNLOTable, self).modify_argument_parser(parser, args)

		self.input_options.add_argument('--pdf-sets', type=str, nargs="+", default=['CT10nlo.LHgrid'],
			help="PDF sets. [Default: %(default)s]")
		self.input_options.add_argument('--members', type=int, nargs="+", default=[0],
			help="PDF set members. [Default: %(default)s]")


	def prepare_args(self, parser, plotData):
		super(InputFastNLOTable, self).prepare_args(parser, plotData)

		# auto determine nicks
		l = ["files", "pdf_sets", "members"]
		for item in l:
			l2 = l[:]
			l2.remove(item)
			if len(plotData.plotdict[item]) > 1 and max([len(plotData.plotdict[x]) for x in l2]) == 1:
				plotData.plotdict['nicks'] = [str(x) for x in plotData.plotdict[item]]

		self.prepare_list_args(plotData, ["nicks", "files", "pdf_sets", "members"])
		self.prepare_nicks(plotData)

		# set default for output filename
		if all([item == [None] for item in [plotData.plotdict[item2+"_expressions"] for item2 in ['x', 'y', 'z']]]) and plotData.plotdict["filename"] == None:
			plotData.plotdict["filename"] = "fastnlo"


	def run(self, plotData):
		for nick, filename, pdfset, member in zip(
				plotData.plotdict['nicks'],
				plotData.plotdict['files'],
				plotData.plotdict['pdf_sets'],
				plotData.plotdict['members']
		):
			fnlo = fastNLOLHAPDF(filename[0])
			fnlo.SetLHAPDFFilename(pdfset)
			fnlo.SetLHAPDFMember(member)
			fnlo.CalcCrossSection()

			# create histogram
			x_binning = sorted(list(set([item for sublist in fnlo.GetDim0BinBounds() for item in sublist])))
			root_histogram = ROOT.TH1D(str(member),str(member),len(x_binning)-1, array('d', x_binning))

			# fill values for central xsec
			xs = np.array(fnlo.GetCrossSection())
			xs[xs <= 0.] = 0.  # ?
			for i in range(0, fnlo.GetNDim0Bins()):
				root_histogram.SetBinContent(i+1, xs[i])

			plotData.plotdict.setdefault("root_objects", {})[nick] = root_histogram
