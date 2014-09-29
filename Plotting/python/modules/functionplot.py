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
		                                help="Comma-Separated function parameters for functions given with --function.")
		self.function_options.add_argument("--function-labels", type=str, nargs="+", default=None,
		                                help="Labels for functions")
		self.function_options.add_argument("--function-colors", type=str, nargs="+", default="black",
		                                help="Linecolor for functions")
		self.function_options.add_argument("--function-linestyles", type=str, nargs="+", default="-",
		                                help="Linestyle for functions")
		self.function_options.add_argument("--function-linewidths", type=str, nargs="+", default="2",
		                                help="Line thickness for functions")

	def prepare_args(self, parser, plotData):
		super(FunctionPlot, self).prepare_args(parser, plotData)
		self.prepare_list_args(plotData, ["functions", "function_parameters", "function_labels", 
		                                  "function_colors", "function_linestyles", "function_linewidths"])

	def run(self, plotData=None):
		super(FunctionPlot, self).run()
		if plotData.plotdict["functions"] != None:
			print plotData.plotdict["functions"]
			print plotData.plotdict["function_parameters"]
			function_parameters = []
			for parameter_set in plotData.plotdict["function_parameters"]:
				function_parameters.append(parameter_set.split(","))
			print function_parameters
			x_min = 0
			x_max = 120 # todo: limits aus x-lims
			plotData.plotdict["root_functions"] = []
			for i, function in enumerate(plotData.plotdict["functions"]):
				root_function_name = "fit_func" + str(i)
				root_function = ROOT.TF1(root_function_name, function, x_min, x_max)
				for j, parameter in enumerate(function_parameters[i]):
					print j
					print parameter
					root_function.SetParameter(j, float(parameter))
				plotData.plotdict["root_functions"].append(root_function)



