# -*- coding: utf-8 -*-

"""
	Creates ROOT histograms by evaluating fastNLO tables.
	PDF or Scale uncertainties can also be evaluated.
	Usage: 	--input-modules InputFastNLO --files fastnlo_table.txt
			[--members 1 2 3 ... --pdf-sets CT10nlo ...]
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

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
		self.fastnlo_options.add_argument('--fastnlo-nicks', type=str, nargs="+", default=[None],
			help="fastnlo nick. [Default: %(default)s]")
		self.fastnlo_options.add_argument('--members', type=int, nargs="+", default=[0],
			help="PDF set members. [Default: %(default)s]")
		self.fastnlo_options.add_argument('--uncertainty-style', type=str, default=None,
			help="Uncertainty style. Choices for PDF: \
			kPDFNone, kHessianSymmetric, kHessianAsymmetric, \
			kHessianAsymmetricMax, kHessianCTEQCL68, kMCSampling, kHeraPDF10. \
			Choices for scale: \
			kScaleNone, kSymmetricTwoPoint, kAsymmetricSixPoint."
		)
		self.fastnlo_options.add_argument('--uncertainty-type', type=str, default=None,
			help="Add PDF or scale uncertainties. [Default: %(default)s]")
		self.fastnlo_options.add_argument('--k-factors', type=bool, nargs="+", default=[False],
			help="k factors instead of actual cross sections. [Default: %(default)s]")

	def prepare_args(self, parser, plotData):
		if isinstance(plotData.plotdict['fastnlo_files'], basestring):
			plotData.plotdict['fastnlo_files'] = [plotData.plotdict['fastnlo_files']]
		self.prepare_list_args(plotData, ["fastnlo_files", "pdf_sets", "members", "fastnlo_nicks", "k_factors"])


	def run(self, plotData):
		import fastnlo
		
		for filename, pdfset, member, nick, kfactor, unctype, uncstyle in zip(
				plotData.plotdict['fastnlo_files'],
				plotData.plotdict['pdf_sets'],
				plotData.plotdict['members'],
				plotData.plotdict['fastnlo_nicks'],
				plotData.plotdict['k_factors'],
				plotData.plotdict['uncertainty_type'],
				plotData.plotdict['uncertainty_style'],
		):
			fnlo = fastnlo.fastNLOLHAPDF(str(filename))
			fnlo.SetLHAPDFFilename(str(pdfset))
			fnlo.SetLHAPDFMember(member)
			fnlo.UseHoppetScaleVariations(True)
			fnlo.CalcCrossSection()

			x_binning = sorted(list(set([item for sublist in fnlo.GetDim0BinBounds() for item in sublist])))

			if unctype is None:
				# create histogram
				root_object = ROOT.TH1D(str(member),str(member),len(x_binning)-1, array('d', x_binning))

				# fill values for central xsec
				xs = np.array( getattr(fnlo, ("GetKFactors" if kfactor else "GetCrossSection"))() )
				xs[xs <= 0.] = 0.  # ?
				for i in range(0, fnlo.GetNDim0Bins()):
					root_object.SetBinContent(i+1, xs[i])
			else:
				# if uncertainties should be calculated, TGraphAsymmErrors must be used
				if kfactor:
					cross_sections, error_up, error_down = fnlo.GetKFactors(), [0.]*len(fnlo.GetKFactors()), [0.]*len(fnlo.GetKFactors())
				else:
					cross_sections, error_up, error_down = (getattr(fnlo, "Get{}UncertaintyVec".format(unctype)))(getattr(fastnlo, uncstyle))
				root_object = ROOT.TGraphAsymmErrors(len(cross_sections))
				for i, xs in enumerate(cross_sections):
					x_center = 0.5*(x_binning[i] + x_binning[i+1])
					root_object.SetPoint(i, x_center, xs)
					root_object.SetPointEYhigh(i, xs*error_up[i])
					root_object.SetPointEYlow(i, xs*abs(error_down[i]))
					root_object.SetPointEXlow(i, x_center-x_binning[i])
					root_object.SetPointEXhigh(i, x_binning[i+1]-x_center)

			# append nick and histo to plotdict
			if nick is None:
				nick = "_".join([filename, pdfset, str(member)])
			plotData.plotdict.setdefault("nicks", []).append(nick)
			plotData.plotdict.setdefault("root_objects", {})[nick] = root_object
