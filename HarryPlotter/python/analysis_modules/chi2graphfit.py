
import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array
import hashlib

import ROOT

import Artus.HarryPlotter.analysisbase as analysisbase


# https://root-forum.cern.ch/t/how-to-pass-fcn-func-defined-on-python-side-to-root-fitter/11168
class Chi2Function(ROOT.TPyMultiGenFunction):

	def __init__(self, graph_points, chi2_formula):
		super(Chi2Function, self).__init__(self)
		self.graph_points = graph_points
		self.chi2_formula = chi2_formula

	def NDim(self):
		return self.chi2_formula.GetNpar()

	def DoEval(self, parameters):
		return sum([self.chi2_formula.EvalPar(point, parameters) for point in self.graph_points])


class Chi2GraphFit(analysisbase.AnalysisBase):
	"""Fit points of 2D graphs with arbitrary chi2 function."""
	
	def __init__(self):
		super(Chi2GraphFit, self).__init__()

	def modify_argument_parser(self, parser, args):
		self.function_options = parser.add_argument_group("{} options".format(self.name()))
		self.function_options.add_argument("--graph-nicks", type=str, nargs="+",
		                                   help="Nick names of TGraph objects to fit.")
		self.function_options.add_argument("--chi2-functions", type=str, nargs="+",
		                                   help="Chi2 functions for fitting in TFormula syntax.")
		self.function_options.add_argument("--chi2-start-parameters", type=str, nargs="+",
		                                   help="Starting parameter values (whitespace separated). This argument defines also the number of parameters.")
#		self.function_options.add_argument("--chi2-parameter-ranges", type=str, nargs="+",
#		                                   help="Parameter ranges in the format \"begin,end\" (whitespace separated).")

	def prepare_args(self, parser, plotData):
		super(Chi2GraphFit, self).prepare_args(parser, plotData)
#		self.prepare_list_args(plotData, ["graph_nicks", "chi2_functions", "chi2_start_parameters", "chi2_parameter_ranges"])
		self.prepare_list_args(plotData, ["graph_nicks", "chi2_functions", "chi2_start_parameters"])
		
#		for index, (chi2_start_parameters, chi2_parameter_ranges) in enumerate(zip(*[plotData.plotdict[k] for k in ["chi2_start_parameters", "chi2_parameter_ranges"]])):
		for index, chi2_start_parameters in enumerate(plotData.plotdict["chi2_start_parameters"]):
			if chi2_start_parameters is None:
				log.error("Parameter --chi2-start-parameters missing for iteration {index}!".format(index=index))
			else:
				plotData.plotdict["chi2_start_parameters"][index] = array.array("d", map(float, chi2_start_parameters.split()))
			
#			if chi2_parameter_ranges is None:
#				log.error("Parameter --chi2-parameter-ranges missing for iteration {index}!".format(index=index))
#			else:
#				plotData.plotdict["chi2_parameter_ranges"][index] = map(lambda item: map(float, item.split(",")), chi2_parameter_ranges.split())

	def run(self, plotData=None):
		super(Chi2GraphFit, self).run()
		
#		for index, (graph_nick, chi2_function, chi2_start_parameters, chi2_parameter_ranges) in enumerate(zip(*[plotData.plotdict[k] for k in ["graph_nicks", "chi2_functions", "chi2_start_parameters", "chi2_parameter_ranges"]])):
		for index, (graph_nick, chi2_function, chi2_start_parameters) in enumerate(zip(*[plotData.plotdict[k] for k in ["graph_nicks", "chi2_functions", "chi2_start_parameters"]])):
#			hash_name = hashlib.md5("_".join(map(str, [graph_nick, chi2_function, chi2_start_parameters, chi2_parameter_ranges]))).hexdigest()
			hash_name = hashlib.md5("_".join(map(str, [graph_nick, chi2_function, chi2_start_parameters]))).hexdigest()
			
			graph = plotData.plotdict["root_objects"].get(graph_nick)
			if graph is None:
				log.error("Skip fitting graph \"{graph}\", which does not exist!".format(graph=graph_nick))
			elif (not isinstance(graph, ROOT.TGraph)) or isinstance(graph, ROOT.TGraph2D):
				log.error("Skip fitting graph \"{graph}\", is not a 2D TGraph object!".format(graph=graph_nick))
			else:
				x_values = graph.GetX()
				y_values = graph.GetY()
				points = [array.array("d", [x_values[index], y_values[index]]) for index in xrange(graph.GetN())]
				
				formula = ROOT.TFormula("chi2_formula_"+hash_name, chi2_function)
				
				fitter = ROOT.Fit.Fitter()
				fitter.Config().MinimizerOptions().SetMinimizerType("Minuit2")
				fitter.Config().MinimizerOptions().SetMinimizerAlgorithm("Migrad")
				
				chi2_function  = Chi2Function(points, formula)
				fitter.FitFCN(chi2_function, chi2_start_parameters)
				fitter.Result().Print(ROOT.cout, True)

