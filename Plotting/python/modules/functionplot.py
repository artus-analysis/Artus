import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT
import HarryPlotter.Plotting.analysisbase as analysisbase
import HarryPlotter.Plotting.roottools as roottools

class FunctionPlot(analysisbase.AnalysisBase):
	def __init__(self):
		super(FunctionPlot, self).__init__()

	def modify_argument_parser(self, parser, args):
		self.function_options = parser.add_argument_group("Function draw options")
		self.function_options.add_argument("--functions", type=str, nargs="+", default=None,
						help="Function to include in plot, ROOT syntax.")
		self.function_options.add_argument("--function-parameters", type=str, nargs="+", default=None,
						help="Comma-Separated function parameters for functions given with --function. If a fit is performed, these are the starting parameters for the fit.")
		self.function_options.add_argument("--function-labels", type=str, nargs="+", default=None,
						help="Labels for functions")
		self.function_options.add_argument("--function-colors", type=str, nargs="+", default="black",
						help="Linecolor for functions")
		self.function_options.add_argument("--function-linestyles", type=str, nargs="+", default="-",
						help="Linestyle for functions")
		self.function_options.add_argument("--function-linewidths", type=str, nargs="+", default="2",
						help="Line thickness for functions")
		self.function_options.add_argument("--function-fit", type=str, nargs="+", default=None,
						help="Fit function to histogram with nickname was argument")
		self.function_options.add_argument("--function-ranges", type=str, nargs="+", default=None,
						help="Function range. Default is whole plot range if histogram is drawn. Format x_min,x_max.")

	def prepare_args(self, parser, plotData):
		super(FunctionPlot, self).prepare_args(parser, plotData)

		self.prepare_list_args(plotData, ["functions", "function_parameters", "function_labels", 
						  "function_colors", "function_linestyles", "function_linewidths", "function_fit", "function_ranges"])
		tmp_x_range = []
		for x_range in plotData.plotdict["function_ranges"]:
			if x_range==None:
				log.debug("Trying to determine the function range automatically")
				if plotData.plotdict["x_lims"]!=None:
					tmp_x_range.append(plotData.plotdict["x_lims"])
				elif plotData.plotdict["root_histos"] != None:
					x_high = []
					x_low = []
					for key, histo in plotData.plotdict["root_histos"].iteritems():
						x_high.append( histo.GetXaxis().GetXmax() )
						x_low.append( histo.GetXaxis().GetXmin() )
					tmp_x_range.append( [ min(x_low), min(x_high)] )
				else:
					log.fatal("Please provide x-limits for the FunctionPlot Module")
					sys.exit(1)
			else:
				tmp_x_range.append( [float (x) for x in  x_range.split(",")] )
		plotData.plotdict["function_ranges"] = tmp_x_range
		print plotData.plotdict["function_ranges"]
	def run(self, plotData=None):
		super(FunctionPlot, self).run()
		if plotData.plotdict["functions"] != None:
			plotData.plotdict["root_functions"] = []
			indices = range(len(plotData.plotdict["functions"]))
			for i, function, parameters, fit_nickname, x_range in zip(indices, plotData.plotdict["functions"], 
			                                                 plotData.plotdict["function_parameters"],
			                                                 plotData.plotdict["function_fit"],
			                                                 plotData.plotdict["function_ranges"]):
				function_parameters= [float (x) for x in  parameters.split(",")]
				if fit_nickname != None and fit_nickname in plotData.plotdict["root_histos"].keys(): 
					root_histogram = plotData.plotdict["root_histos"][fit_nickname]
					plotData.plotdict["root_functions"].append(self.createFunction(function, x_range[0], x_range[1], 
					                                           function_parameters, 
					                                           nick=fit_nickname, 
					                                           root_histogram=root_histogram))
				else: 
					plotData.plotdict["root_functions"].append(self.createFunction(function, x_range[0], x_range[1], function_parameters))


	def createFunction(self, function, x_min, x_max, start_parameters, nick="", root_histogram=None):
		"""
		creates a TF1 function from input formula

		does the fit and adds the fitted function to the dictionary
		"""
		formula_name = "function_" + nick
		# todo: ensure to have as many start parameters as parameters in formula
		root_function = ROOT.TF1(formula_name, function, x_min, x_max)
		# set parameters for fit or just for drawing the function
		for parameter_index in range(root_function.GetNpar()):
			root_function.SetParameter(parameter_index, start_parameters[parameter_index])
		if root_histogram != None:
			root_histogram.Fit(formula_name)
		return root_function


	def getParameters(self, root_function):
		parameters = []
		parameter_errors = []
		chi_square = root_function.GetChisquare()
		for parameter_index in range(root_function.GetNpar()):
			parameters.append(root_function.GetParameter(parameter_index))
			parameter_errors.append(root_function.GetParError(parameter_index))
		return parameters, parameter_errors, chi_square
