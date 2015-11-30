# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class RemoveErrors(analysisbase.AnalysisBase):
	"""Remove errors of histograms."""
	
	def __init__(self):
		super(RemoveErrors, self).__init__()

	def modify_argument_parser(self, parser, args):
		super(RemoveErrors, self).modify_argument_parser(parser, args)

		self.RemoveErrors_options = parser.add_argument_group("RemoveErrors options")
		self.RemoveErrors_options.add_argument("--remove-error-nicks", type=str, nargs="+",
				help="Nick names of the histograms/graphs to be modified in place.")
	
	def prepare_args(self, parser, plotData):
		super(RemoveErrors, self).prepare_args(parser, plotData)
		if plotData.plotdict["remove_error_nicks"] is None:
			plotData.plotdict["remove_error_nicks"] = plotData.plotdict["nicks"]
	
	def run(self, plotData=None):
		super(RemoveErrors, self).run(plotData)

		for nick in plotData.plotdict["remove_error_nicks"]:
			root_object = plotData.plotdict["root_objects"][nick]
			RemoveErrors.remove_errors(root_object)
	
	@staticmethod
	def remove_errors(root_object):
		if isinstance(root_object, ROOT.TH1) and not isinstance(root_object, ROOT.TProfile):
			for x_bin in xrange(1, root_object.GetNbinsX()+1):
				for y_bin in xrange(1, root_object.GetNbinsY()+1):
					for z_bin in xrange(1, root_object.GetNbinsZ()+1):
						global_bin = root_object.GetBin(x_bin, y_bin, z_bin)
						root_object.SetBinError(global_bin, 0.0)
		
		elif isinstance(root_object, ROOT.TGraph) and (not isinstance(root_object, ROOT.TGraph2D)):
			has_errors = isinstance(root_object, ROOT.TGraphErrors)
			has_asymm_errors = isinstance(root_object, ROOT.TGraphAsymmErrors)
			for point in xrange(root_object.GetN()):
				if has_asymm_errors:
					root_object.SetPointError(point, 0.0, 0.0, 0.0, 0.0)
				elif has_errors:
					root_object.SetPointError(point, 0.0, 0.0)
		
		else:
			log.warning("Module \"{name}\" does not support object of type \"{object_type}\"!".format(
					name=self.name(),
					object_type=str(type(root_object))
			))

