# -*- coding: utf-8 -*-

"""
"""

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import HarryPlotter.Plotting.plotbase as plotbase
import HarryPlotter.Plotting.roottools as roottools

import ROOT


class ExportRoot(plotbase.PlotBase):
	def __init__(self):
		super(ExportRoot, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(ExportRoot, self).modify_argument_parser(parser, args)
		
		self.formatting_options.add_argument("-L", "--labels", type=str, nargs="+",
		                                     help="Paths for the ROOT objects in the file.")
		                                     
		self.output_options.set_defaults(formats=["root"])
	
	def prepare_args(self, parser, plotData):
		if not "root" in plotData.plotdict["formats"]:
			plotData.plotdict["formats"].append("root")
	
		super(ExportRoot, self).prepare_args(parser, plotData)
		
		for index, (nick, path) in enumerate(zip(plotData.plotdict["nicks"], plotData.plotdict["labels"])):
			if path == None:
				plotData.plotdict["labels"][index] = nick
			
		self.output_filenames = [filename for filename in plotData.plotdict["output_filenames"] if filename.endswith(".root")]
		plotData.plotdict["output_filenames"] = [filename for filename in plotData.plotdict["output_filenames"] if not filename.endswith(".root")]
	
	def run(self, plotdict=None):
		super(ExportRoot, self).run(plotdict)
		
	def calculate_ratios(self, plotData):
		pass
		
	def create_canvas(self, plotData):
		self.root_output_files = [ROOT.TFile(output_filename, "RECREATE") for output_filename in self.output_filenames]
		
	def prepare_histograms(self, plotData):
		pass
		
	def make_plots(self, plotData):
		for root_output_file in self.root_output_files:
			root_output_file.cd()
			
			for nick, path in zip(plotData.plotdict["nicks"], plotData.plotdict["labels"]):
				root_object = plotData.plotdict["root_histos"][nick]
				roottools.RootTools.write_object(root_output_file, root_object, path)
		
	def modify_axes(self, plotData):
		pass
		
	def add_labels(self, plotData):
		pass
		
	def add_texts(self, plotData):
		pass
		
	def save_canvas(self, plotData):
		for root_output_file, output_filename in zip(self.root_output_files, self.output_filenames):
			root_output_file.Write()
			root_output_file.Close()
			log.info("Exported root objects to \"%s\"." % output_filename)
		
	def plot_end(self, plotData):
		pass
		

