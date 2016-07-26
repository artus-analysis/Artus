# -*- coding: utf-8 -*-

"""
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import glob
import os
import sys

import Artus.HarryPlotter.inputbase as inputbase


class InputFile(inputbase.InputBase):
	def __init__(self):
		super(InputFile, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		super(InputFile, self).modify_argument_parser(parser, args)
		
		self.input_options.add_argument("-i", "--files", type=str, nargs="+",
		                                help="Input (root) file(s).")
		self.input_options.add_argument("-d", "--directories", type=str, nargs="+", default=[None],
		                                help="Input directories, that are put before the values of the -i/--files option.")
	
	def prepare_args(self, parser, plotData):
		super(InputFile, self).prepare_args(parser, plotData)
		
		if plotData.plotdict["files"] is None:
			log.critical(self.name() + ": No input files given!")
			sys.exit(1)

		self.prepare_list_args(plotData, ["nicks", "x_expressions", "y_expressions", "z_expressions", "x_bins", "y_bins", "z_bins", "scale_factors", "files", "directories"], help="Input file options")
		
		# prepare files
		for index, (file_args, directories) in enumerate(zip(plotData.plotdict["files"], plotData.plotdict["directories"])):
			paths_before_globbing = []
			files = []
			# treat inputs separately, for which globbing does not work
			if any([file_args.startswith(protocol+"://") for protocol in ["root", "dcap"]]):
				files.append(file_args)
			else:
				for file_arg in file_args.split():
					for directory in directories.split() if directories else [None]:
						paths_before_globbing.append(os.path.expandvars(os.path.join(directory, file_arg) if directory else file_arg))
						new_files = glob.glob(paths_before_globbing[-1])
						if len(new_files) == 0:
							log.error("Input argument %d (%s) does not contain any existing files!" % (index, paths_before_globbing[-1]))
						files.extend(new_files)
				if len(files) == 0:
					log.fatal("No inputs forund for argument %d!" % index)
					sys.exit(1)
			plotData.plotdict["files"][index] = files
	
	def run(self, plotData):
		super(InputFile, self).run(plotData)

