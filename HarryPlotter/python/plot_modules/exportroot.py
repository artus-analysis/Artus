# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.HarryPlotter.plotbase as plotbase
import Artus.HarryPlotter.plotdata as plotdata
import Artus.HarryPlotter.utility.roottools as roottools

import ROOT


class ExportPlotContainer(plotdata.PlotContainer):
	def __init__(self, root_files=None):
		self.root_files = root_files
	
	def finish(self):
		pass
	
	def save(self, filename):
		root_file = self.root_files.get(filename, None)
		if not root_file is None:
			root_file.Write()
			root_file.Close()


class ExportRoot(plotbase.PlotBase):
	"""Write all Root objects in plotdict to a Root file."""

	def __init__(self):
		super(ExportRoot, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(ExportRoot, self).modify_argument_parser(parser, args)
		
		self.root_export_options = parser.add_argument_group("ROOT Export options")
		self.root_export_options.add_argument("--file-mode", default="RECREATE",
		                                      help="Mode for opening ROOT output file. [Default: %(default)s]")
		
		self.formatting_options.add_argument("-L", "--labels", type=str, nargs="+",
		                                     help="Paths for the ROOT objects in the file.")
		
		self.output_options.set_defaults(formats=["root"])
	
	def prepare_args(self, parser, plotData):
		plotData.plotdict["formats"] = ["root"]
	
		super(ExportRoot, self).prepare_args(parser, plotData)
		
		for index, (nick, path) in enumerate(zip(plotData.plotdict["nicks"], plotData.plotdict["labels"])):
			if path == None:
				plotData.plotdict["labels"][index] = nick
	
	def run(self, plotdict=None):
		super(ExportRoot, self).run(plotdict)
		
	def calculate_ratios(self, plotData):
		pass
		
	def create_canvas(self, plotData):
		plotData.plot = ExportPlotContainer(
				{output_filename : ROOT.TFile(output_filename, plotData.plotdict["file_mode"]) for output_filename in plotData.plotdict["output_filenames"]}
		)
		
	def prepare_histograms(self, plotData):
		pass
		
	def make_plots(self, plotData):
		for root_filename, root_file in plotData.plot.root_files.iteritems():
			root_file.cd()
			
			for nick, path in zip(plotData.plotdict["nicks"], plotData.plotdict["labels"]):
				root_object = plotData.plotdict["root_objects"][nick]
				roottools.RootTools.write_object(root_file, root_object, path)
		
	def modify_axes(self, plotData):
		pass
		
	def add_labels(self, plotData):
		pass
		
	def add_texts(self, plotData):
		pass
		
	def plot_end(self, plotData):
		pass
		

