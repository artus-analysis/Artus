# -*- coding: utf-8 -*-

"""
"""

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT
import hashlib
import HarryPlotter.Plotting.analysisbase as analysisbase
import HarryPlotter.Plotting.roottools as roottools

class ProjectByFit(analysisbase.AnalysisBase):
	def __init__(self):
		super(ProjectByFit, self).__init__()

	def run(self, plotData=None):
		super(ProjectByFit, self).run()

		for root_files, folders, x_expression, y_expression, weight, nick in zip(*
			[plotData.plotdict[key] for key in ["files", "folders", "x_expressions", "y_expressions", "weights", "nicks"]]):

			root_tools = roottools.RootTools()
			# check whether to read from tree or directly from histograms
			root_object_type = roottools.RootTools.check_type(root_files, folders)
			root_tree_chain = None
			root_histogram = None
			result_histogram_name = "histogram_{0}".format(hashlib.md5("_".join([str(root_files),
                                                str(folders),
                                                x_expression, y_expression,
                                                str(plotData.plotdict["x_bins"]), str(plotData.plotdict["y_bins"]),
                                                str(weight)])).hexdigest())
			binning_string, bin_edges = root_tools.prepare_binning(plotData.plotdict["x_bins"])
			if bin_edges==None:
				log.fatal("Please provide manual bin edges for this plotting module")
				sys.exit(1)
			print bin_edges
			print str(len(bin_edges)-1)
			print str(bin_edges[0])
			print str( bin_edges[-1])
			result_histogram = ROOT.TH1F(result_histogram_name, "", len(bin_edges)-1, bin_edges[0], bin_edges[-1])
			for i in range(len(bin_edges)-1):
				left_edge = str(bin_edges[i])
				right_edge = str(bin_edges[i+1])
				slice_weight = "(" + weight + ") * ( " + x_expression + ">="+ left_edge + ") * (" + x_expression + " < " + right_edge + " )"
				print slice_weight
				if root_object_type == ROOT.TTree:
					variable_expression = y_expression

					option = plotData.plotdict["root_histogram_draw_options"]       
					root_tree_chain, root_histogram = root_tools.histogram_from_tree(
							root_files, folders,
								x_expression, y_expression, None,
							x_bins=plotData.plotdict["x_bins"],
							y_bins=plotData.plotdict["y_bins"],
							z_bins=[25],
							weight_selection=weight, option=option, name=None
					)
					function = "([0]*exp(-0.5*((x-[1])/[2])^2))" #todo
					x_min = 0 #todo
					x_max = 150 #todo
					start_parameters = [1000, 10, 20] #todo
					parameters, parameter_errors, chi_square = self.fitFunction(root_histogram=root_histogram, function=function, bin_number=i, x_min=x_min, x_max=x_max, start_parameters=start_parameters)
					print parameters
					print parameter_errors
					print chi_square
					result_histogram.SetBinContent(i, parameters[1]) # todo
					result_histogram.SetBinError(i, parameter_errors[1]) # todo mit obigem
				else:
					log.fatal("The ProjectByFit Module up to now only can handle TTrees as Input")
					sys.exit(1)
			plotData.plotdict["root_histos"] = {'nick0' : result_histogram}
	def fitFunction(self, root_histogram, function, bin_number, x_min, x_max, start_parameters):
		"""
		Gets the root histogram and the desired fitting function.

		Performs the fit, returns the fit results and the fitted function
		"""
		n_parameters = self.number_of_parameters(function)
		fit_name = "fit_function_bin" + str(bin)
		# todo: ensure to have as many start parameters as parameters in formula
		fit_function = ROOT.TF1(fit_name, function, x_min, x_max)
		for parameter_index in range(n_parameters):
			fit_function.SetParameter(start_parameters[parameter_index], 1)
		root_histogram.Fit(fit_name)

		#print "results"
		#print fit_function.GetChisquare() # create control plot from chisquare?
		chi_square = fit_function.GetChisquare()
		parameters = []
		parameter_errors = []
		for parameter_index in range(n_parameters):
			#print fit_function.GetParameter(parameter_index)
			parameters.append(fit_function.GetParameter(parameter_index))
			#print fit_function.GetParError(parameter_index)
			parameter_errors.append(fit_function.GetParError(parameter_index))
		return parameters, parameter_errors, chi_square


	def number_of_parameters(self, formula):
		opening = formula.count('[')
		closing = formula.count(']')
		if opening == closing:
			return opening
		else:
			log.fatal("Invalid fit formula! " + str(opening) + " opening and " + str(closing) + " brackets.")
			sys.exit(1)


		# cross-checks:
			# fit pro slice speichern
			# chisquare pro fit speichern/plotten

		# konfigurierbare fit-func
		# fit dran machen
		# fit parameter in dict speichern?
