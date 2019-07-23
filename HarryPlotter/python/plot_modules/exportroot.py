# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.Utility.jsonTools as jsonTools
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
		self.root_export_options.add_argument(
				"--file-mode", default="RECREATE",
				help="Mode for opening ROOT output file. [Default: %(default)s]"
		)
		
		self.formatting_options.add_argument(
				"-L", "--labels", type=str, nargs="+",
				help="Paths for the ROOT objects in the file."
		)
		self.formatting_options.add_argument(
				"--nicks-instead-labels", nargs="?", type="bool", default=False, const=True,
				help="Use the nicks to specify the paths of the ROOT objects in the file. [Default: %(default)s]"
		)
		
		self.formatting_options.add_argument(
				"--custom-rebin", nargs="+", type=float, default=None,
				help="Rebins before exporting in the file. Provided value is a list of bin edges. [Default: %(default)s]"
		)
		
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
		pass
		
	def make_plots(self, plotData):
		for root_filename, root_file in plotData.plot.root_files.iteritems():
			root_file.cd()
			
			for nick, path in zip(plotData.plotdict["nicks"], plotData.plotdict["labels"]):
				root_object = plotData.plotdict["root_objects"][nick]
				
				if plotData.plotdict["custom_rebin"] is not None:
					root_object = roottools.RootTools.rebin_root_histogram(
						root_object,
						rebinningX=plotData.plotdict["custom_rebin"])
				
				if plotData.plotdict["nicks_instead_labels"]:
					path = nick
				roottools.RootTools.write_object(root_file, root_object, path)
				
				metadata = plotData.metadata.get(nick, None)
				if not metadata is None:
					root_metadata = {key : value for key, value in metadata.iteritems() if isinstance(value, ROOT.TObject)}
					json_metadata = jsonTools.JsonDict({key : value for key, value in metadata.iteritems() if not isinstance(value, ROOT.TObject)})
					
					for key, meta_root_object in root_metadata.iteritems():
						roottools.RootTools.write_object(root_file, meta_root_object, path+"_"+key)
					
					if len(json_metadata) > 0:
						meta_json_object = ROOT.TObjString(json_metadata.toString())
						roottools.RootTools.write_object(root_file, meta_json_object, path+"_metadata")
	
	def modify_axes(self, plotData):
		pass
		
	def add_labels(self, plotData):
		pass
		
	def add_texts(self, plotData):
		pass
		
	def plot_end(self, plotData):
		pass
		

