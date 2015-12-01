# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


class ScaleErrors(analysisbase.AnalysisBase):
	"""Scale errors of histograms."""
	
	def __init__(self):
		super(ScaleErrors, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(ScaleErrors, self).modify_argument_parser(parser, args)
		
		self.ScaleErrors_options = parser.add_argument_group("ScaleErrors options")
		self.ScaleErrors_options.add_argument("--scale-error-nicks", type=str, nargs="+",
				help="Nick names of the histograms/graphs to be modified in place. [Default: modify all histograms]")
		self.ScaleErrors_options.add_argument("--scale-error-factors", type=float, nargs="+", default=[0.0],
				help="Scale factors for errors. [Default: %(default)s]")
	
	def prepare_args(self, parser, plotData):
		super(ScaleErrors, self).prepare_args(parser, plotData)
		if plotData.plotdict["scale_error_nicks"] is None:
			plotData.plotdict["scale_error_nicks"] = plotData.plotdict["nicks"]
			
		self.prepare_list_args(plotData, ["scale_error_nicks", "scale_error_factors"])
	
	def run(self, plotData=None):
		super(ScaleErrors, self).run(plotData)
		
		for nick, scale_factor in zip(plotData.plotdict["scale_error_nicks"], plotData.plotdict["scale_error_factors"]):
			root_object = plotData.plotdict["root_objects"][nick]
			ScaleErrors.scale_errors(root_object, scale_factor)
	
	@staticmethod
	def scale_errors(root_object, scale_factor=0.0):
		if isinstance(root_object, ROOT.TH1) and not isinstance(root_object, ROOT.TProfile):
			for x_bin in xrange(1, root_object.GetNbinsX()+1):
				for y_bin in xrange(1, root_object.GetNbinsY()+1):
					for z_bin in xrange(1, root_object.GetNbinsZ()+1):
						global_bin = root_object.GetBin(x_bin, y_bin, z_bin)
						root_object.SetBinError(global_bin, scale_factor*root_object.GetBinError(global_bin))
		
		elif isinstance(root_object, ROOT.TGraph) and (not isinstance(root_object, ROOT.TGraph2D)):
			for point in xrange(root_object.GetN()):
				if isinstance(root_object, ROOT.TGraphAsymmErrors):
					root_object.SetPointError(point, scale_factor*GetErrorXlow(point), scale_factor*GetErrorXhigh(point),
					                          scale_factor*GetErrorYlow(point), scale_factor*GetErrorYhigh(point))
				elif isinstance(root_object, ROOT.TGraphErrors):
					root_object.SetPointError(point, scale_factor*GetErrorX(point), scale_factor*GetErrorY(point))
		
		else:
			log.warning("Module \"{name}\" does not support object of type \"{object_type}\"!".format(
					name=self.name(),
					object_type=str(type(root_object))
			))

