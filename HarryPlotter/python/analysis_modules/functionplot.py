import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT
import Artus.HarryPlotter.analysisbase as analysisbase
import hashlib
import array

class FunctionPlot(analysisbase.AnalysisBase):
	def __init__(self):
		super(FunctionPlot, self).__init__()

	def modify_argument_parser(self, parser, args):
		self.function_options = parser.add_argument_group("Function draw options")
		self.function_options.add_argument("--functions", type=str, nargs="+", default=None,
						help="Function to include in plot, ROOT syntax.")
		self.function_options.add_argument("--function-nicknames", type=str, nargs="+",
						help="Nickname of the function")
		self.function_options.add_argument("--function-parameters", type=str, nargs="+", default=None,
						help="Comma-Separated function parameters for functions given with --function. If a fit is performed, these are the starting parameters for the fit.")
		self.function_options.add_argument("--function-fit", type=str, nargs="+", default=None,
						help="List of nicknames of histograms one wants to fit.")
		self.function_options.add_argument("--function-ranges", type=str, nargs="+", default=None,
						help="Function range. Default is whole plot range if histogram is drawn. Format x_min,x_max.")
		self.function_options.add_argument("--fit-backend", type=str, nargs="+", default="ROOT",
						help="Fit backend. ROOT and RooFit are available. Check sourcecode which parts of RooFit are implemented. [Default: %(default)s]")
		self.function_options.add_argument("--function-display-result", action='store_true',
						help="Display the parameters of the fit on the plots")
		self.function_options.add_argument("--function-fit-parameter-names", type=str, nargs="+", default=["Parameter"],
						help="Names of the parameters (e.g. 'Slope'). Only relevant if --function-display-result is True. [Default: %(default)s]")

		self.function_options.add_argument("--function-collect-result", type=str, nargs="+", default=["None"],
						help="Collect fit results in a histogram. Save either the parameter with the given number or the Chi2. [Default: %(default)s]")

	def prepare_args(self, parser, plotData):
		self.prepare_list_args(plotData, ["functions", "function_parameters", "function_nicknames",
						                  "function_fit", "function_ranges", "fit_backend", "function_collect_result"])
		tmp_x_range = []
		for x_range in plotData.plotdict["function_ranges"]:
			if x_range==None:
				log.debug("Trying to determine the function range automatically")
				if plotData.plotdict["x_lims"]!=None:
					tmp_x_range.append(plotData.plotdict["x_lims"])
				elif plotData.plotdict["root_objects"] != None:
					x_high = []
					x_low = []
					for key, histo in plotData.plotdict["root_objects"].iteritems():
						x_high.append( histo.GetXaxis().GetXmax() )
						x_low.append( histo.GetXaxis().GetXmin() )
					tmp_x_range.append( [ min(x_low), min(x_high)] )
				else:
					log.fatal("Please provide fucntion ranges for the FunctionPlot Module")
					sys.exit(1)
			else:
				tmp_x_range.append( [float (x) for x in  x_range.split(",")] )
		plotData.plotdict["function_ranges"] = tmp_x_range
		
		plotData.plotdict["function_nicknames"] = [nick if nick != None else ("function_nick%d" % index) for index, nick in enumerate(plotData.plotdict["function_nicknames"])]
		plotData.plotdict["nicks"] += plotData.plotdict["function_nicknames"]

		tmp_function_parameters = []
		for function_parameter in plotData.plotdict["function_parameters"]:
			tmp_function_parameters.append([float (x) for x in  function_parameter.split(",")])
		plotData.plotdict["function_parameters"] = tmp_function_parameters

		super(FunctionPlot, self).prepare_args(parser, plotData)

		self.n_functions = len(plotData.plotdict["function_collect_result"]) - plotData.plotdict["function_collect_result"].count(None)
		
		if self.n_functions > 0:
			plotData.plotdict["root_objects"]["function_fit_result"] = ROOT.TH1F("function_fit_result", "function_fit_result", self.n_functions, 0, self.n_functions)
			plotData.plotdict["nicks"].append("function_fit_result")

	def run(self, plotData=None):
		super(FunctionPlot, self).run()
		if plotData.plotdict["functions"] == None:
			log.info("You are using the FunctionPlot module. Please provide at least one input function with --function")
			os.exit(1)
		else:
			for i, (function, function_nick, function_parameters, fit_nickname, x_range, collect_result, fit_backend) in enumerate(zip( 
			                                                 plotData.plotdict["functions"], 
			                                                 plotData.plotdict["function_nicknames"],
			                                                 plotData.plotdict["function_parameters"],
			                                                 plotData.plotdict["function_fit"],
			                                                 plotData.plotdict["function_ranges"],
			                                                 plotData.plotdict["function_collect_result"],
			                                                 plotData.plotdict["fit_backend"])):
				if fit_nickname != None and fit_nickname in plotData.plotdict["root_objects"].keys(): 
					root_histogram = plotData.plotdict["root_objects"][fit_nickname]
					root_function, fit_result = self.create_function(function, x_range[0], x_range[1],
					                                           function_parameters, 
					                                           nick=fit_nickname, 
					                                           root_histogram=root_histogram,
					                                           fit_backend=fit_backend)
					plotData.plotdict["root_objects"][function_nick] = root_function
					plotData.fit_results[function_nick] = fit_result
					if plotData.plotdict["function_display_result"]:
						self.add_results_text(plotData, function_nick)
				else:
					plotData.plotdict["root_objects"][function_nick] = self.create_tf1(function, x_range[0], x_range[1], function_parameters)

				if collect_result == None:
					pass
				elif collect_result == "Chi2":
					plotData.plotdict["root_objects"]["function_fit_result"].SetBinContent(i+1, plotData.fit_results[function_nick].Chi2())
				elif collect_result.isdigit():
					plotData.plotdict["root_objects"]["function_fit_result"].SetBinContent(i+1, plotData.fit_results[function_nick].Parameter(int(plotData.plotdict["function_collect_result"])))
				
				print "Probability to obtain a Chi2 of " + str(plotData.fit_results[function_nick].Chi2()) + " for an ndf of " + str(plotData.fit_results[function_nick].Ndf()) + " is " + str(ROOT.TMath.Prob(plotData.fit_results[function_nick].Chi2(),plotData.fit_results[function_nick].Ndf()))

	def create_function(self, function, x_min, x_max, start_parameters, nick="", root_histogram=None, fit_backend="ROOT"):
		"""
		creates a function from input formula from either ROOT or RooFit backend.

		does the fit and adds the fitted function to the dictionary
		"""
		fit_result = None
		# todo: ensure to have as many start parameters as parameters in formula
		if fit_backend == "RooFit":
			root_function, mean, sigma, width = self.do_roofit(function, x_min, x_max, start_parameters, root_histogram)
		elif fit_backend == "ROOT":
			root_function, fit_result = self.do_rootfit(function, x_min, x_max, start_parameters, nick, root_histogram)
		else:
			print "No valid fit backend selected"
			exit()
		return root_function, fit_result

	@staticmethod
	def create_tf1(function, x_min, x_max, start_parameters, nick="", root_histogram=None):
		formula_name = ("function_" + nick).format(hashlib.md5("_".join([str(function), str(x_min), str(x_max),
		                                                                str(start_parameters), str(nick), 
		                                                                str(root_histogram.GetName() if root_histogram != None else "")])).hexdigest())
		ret_tf1 = ROOT.TF1(formula_name, function, x_min, x_max)
		ret_tf1.SetParameters(array.array('d', start_parameters))
		return ret_tf1

	@staticmethod
	def do_rootfit(function, x_min, x_max, start_parameters, nick="", root_histogram=None):
		root_function = FunctionPlot.create_tf1(function, x_min, x_max, start_parameters, nick, root_histogram)
		# set parameters for fit or just for drawing the function
		for parameter_index in range(root_function.GetNpar()):
			root_function.SetParameter(parameter_index, start_parameters[parameter_index])
		option = "S L" if("function_fit_result" in root_function.GetName()) else "S"
		fit_result = root_histogram.Fit(root_function.GetName(), option, "", x_min, x_max)
		return root_function, fit_result

	@staticmethod
	def do_roofit(function, x_min, x_max, start_parameters, root_histogram):

		integral = root_histogram.Integral()
		maximum = root_histogram.GetMaximum(integral)
		x = ROOT.RooRealVar("x", "x", x_min, x_max)
		dh = ROOT.RooDataHist("dh", "dh", ROOT.RooArgList(x), root_histogram)

		frame = x.frame()
		ROOT.RooAbsData.plotOn(dh, frame)

		mean  = ROOT.RooRealVar("mean", "mean", start_parameters[0],
			                                    start_parameters[1],
			                                    start_parameters[2])
		sigma = ROOT.RooRealVar("sigma","sigma",start_parameters[3],
			                                    start_parameters[4],
			                                    start_parameters[5])
		width = ROOT.RooRealVar("width","width",start_parameters[6],
			                                    start_parameters[7],
			                                    start_parameters[8])
		if (function == "voigtian"):
			function = ROOT.RooVoigtian("func","func",x,mean,width,sigma)
		elif (function == "breitwiegner"):
			function = ROOT.RooBreitWiegner("func","func",x,mean,sigma)
		elif (function == "gaus"):
			function = ROOT.RooGaussian("func","func",x,mean,sigma)
		else:
			print "selected fit function " + function + " currently not supported from functionplot modul with RooFit backend."
			exit()
		
		filters = function.fitTo(dh,ROOT.RooLinkedList())
		root_function = ROOT.RooCurve(function, x, x_min, x_max, 1000, maximum)
		return root_function, mean, sigma, width

	@staticmethod
	def get_roofit_parameter(function, x_min, x_max, start_parameters, root_histogram, parameter):
		retVal = [0,0,0]
		root_function, retVal[0], retVal[1], retVal[2] = FunctionPlot.do_roofit(function, x_min, x_max, start_parameters, root_histogram)
		return retVal[parameter].getVal(), retVal[parameter].getError()

	def get_parameters(self, root_function):
		parameters = []
		parameter_errors = []
		chi_square = root_function.GetChisquare()
		ndf = root_function.GetNDF()
		for parameter_index in range(root_function.GetNpar()):
			parameters.append(root_function.GetParameter(parameter_index))
			parameter_errors.append(root_function.GetParError(parameter_index))
		return parameters, parameter_errors, chi_square, ndf

	def add_results_text(self, plotData, function_nick):
		"""Add fit result parameter values and chi2 to plotdict['text']"""
		text = "Fit results"
		if len(plotData.plotdict["function_fit"]) > 1:  # only add the nickname if more than one fit
			text += " {}:".format(function_nick)

		# expand paramter_names if necessary
		if len(plotData.plotdict["function_fit_parameter_names"]) < plotData.plotdict["root_objects"][function_nick].GetNpar():
			plotData.plotdict["function_fit_parameter_names"] *= plotData.plotdict["root_objects"][function_nick].GetNpar()/len(plotData.plotdict["function_fit_parameter_names"])
		l = max([len(s) for s in plotData.plotdict["function_fit_parameter_names"]])

		for i_par in range(plotData.plotdict["root_objects"][function_nick].GetNpar()):
			#TODO automatically adjust decimal precision
			text += "\n  ${} = {:.3f} \pm {:.3f}$".format(plotData.plotdict["function_fit_parameter_names"][i_par],
			        plotData.plotdict["root_objects"][function_nick].GetParameter(i_par),
			        plotData.plotdict["root_objects"][function_nick].GetParError(i_par))
		text += "\n  $\chi^2 / \mathit{{n.d.f}} = {:.2f} / {}$".format(
		        plotData.fit_results[function_nick].Chi2(),
		        plotData.fit_results[function_nick].Ndf())
		if plotData.plotdict["texts"] == [None]:
			plotData.plotdict["texts"] = [text]
		else:
			plotData.plotdict["texts"] += [text]
