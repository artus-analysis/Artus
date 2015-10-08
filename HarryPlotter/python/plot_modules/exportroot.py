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
		super(ExportRoot, self).create_canvas(plotData)
		
		plotData.plot = ExportPlotContainer(
				{output_filename : ROOT.TFile(output_filename, plotData.plotdict["file_mode"]) for output_filename in plotData.plotdict["output_filenames"]}
		)
		
	def prepare_histograms(self, plotData):
		# some debug info
		if log.isEnabledFor(logging.DEBUG):
			for index, (nick1, stack1) in enumerate(zip(plotData.plotdict["nicks"], plotData.plotdict["stacks"])):
				log.debug("\nContents of nick %s, stack %s:" % (nick1, stack1))
				plotData.plotdict["root_objects"][nick1].Print("range")
				# print non empty over-/underflow bins separately
				log.debug("\nOver-/underflow bins of nick %s, stack %s:" % (nick1, stack1))
				if plotData.plotdict["root_objects"][nick1].GetDimension() == 1:
					for xbin in range(0, plotData.plotdict["root_objects"][nick1].GetNbinsX() + 2):
						if (xbin == 0 or xbin == plotData.plotdict["root_objects"][nick1].GetNbinsX() + 1):
							if plotData.plotdict["root_objects"][nick1].GetBinContent(xbin) != 0.0:
								print "[%i] = %s" % (xbin, plotData.plotdict["root_objects"][nick1].GetBinContent(xbin))
				elif plotData.plotdict["root_objects"][nick1].GetDimension() == 2:
					for xbin in range(0, plotData.plotdict["root_objects"][nick1].GetNbinsX() + 2):
						for ybin in range(0, plotData.plotdict["root_objects"][nick1].GetNbinsY() + 2):
							if (xbin == 0 or xbin == plotData.plotdict["root_objects"][nick1].GetNbinsX() + 1 or ybin == 0 or ybin == plotData.plotdict["root_objects"][nick1].GetNbinsY() + 1):
								if plotData.plotdict["root_objects"][nick1].GetBinContent(xbin, ybin) != 0.0:
									print "[%i][%i] = %s" % (xbin, ybin, plotData.plotdict["root_objects"][nick1].GetBinContent(xbin, ybin))
		
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
		

