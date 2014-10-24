# -*- coding: utf-8 -*-

"""
"""

import logging
import HarryPlotter.Utility.logger as logger
log = logging.getLogger(__name__)

import collections
import ROOT

import HarryPlotter.Plotting.processor as processor


class InputBase(processor.Processor):
	def __init__(self):
		super(InputBase, self).__init__()
	
	def modify_argument_parser(self, parser, args):
		self.input_options = parser.add_argument_group("Input options")
		self.input_options.add_argument("--nicks", type=str, nargs="+",
		                                 help="Defines nick names for inputs. Inputs with the same nick name will be merged. By default, every input gets a unique nick name.")
		self.input_options.add_argument("-x", "--x-expressions", type=str, nargs="+",
		                                help="x-value(s)")
		self.input_options.add_argument("-y", "--y-expressions", type=str, nargs="+",
		                                help="y-value(s)")
		self.input_options.add_argument("-z", "--z-expressions", type=str, nargs="+",
		                                help="z-value(s)")
		self.input_options.add_argument("-s", "--scale-factors", nargs="+",
		                                help="Scale factors.")
	
	def prepare_args(self, parser, plotData):
		super(InputBase, self).prepare_args(parser, plotData)
		
		self.prepare_list_args(plotData, ["nicks", "x_expressions", "y_expressions", "z_expressions", "scale_factors"])
		
		# prepare nicks
		plotData.plotdict["nicks"] = [nick if nick != None else ("nick%d" % index) for index, nick in enumerate(plotData.plotdict["nicks"])]
		
		# prepare scale factors
		plotData.plotdict["scale_factors"] = [float(scale) if scale != None else 1.0 for scale in plotData.plotdict["scale_factors"]]
		print plotData.plotdict["scale_factors"]
		
		
	
	def run(self, plotData):
		super(InputBase, self).run(plotData)
		
		# scaling histograms
		nick_occurences = {}
		for index, (nick, scale_factor) in enumerate(zip(*[plotData.plotdict[key] for key in ["nicks", "scale_factors"]])):
			root_object = plotData.plotdict["root_objects"][nick]
			if isinstance(root_object, collections.Iterable) and not isinstance(root_object, ROOT.TObject):
				root_object = root_object[nick_occurences.get(nick, 0)]
				nick_occurences[nick] = nick_occurences.get(nick, 0) + 1
			
			if isinstance(root_object, ROOT.TH1):
				root_object.Scale(scale_factor)
			elif scale_factor != 1.0:
				log.warning("Scaling currently only implmented for histograms!")
		
		# merging objects with same nicks
		for nick, root_objects in plotData.plotdict["root_objects"].iteritems():
			if isinstance(root_objects, collections.Iterable) and not isinstance(root_objects, ROOT.TObject):
				merged_object = None
				for root_object in root_objects:
					if isinstance(root_object, ROOT.TH1):
						if merged_object == None:
							merged_object = root_object
						else:
							merged_object.Add(root_object)
					elif scale_factor != 1.0:
						log.warning("Merging currently only implmented for histograms!")
				plotData.plotdict["root_objects"][nick] = merged_object
		
		# remove all nick name copies
		tmp_nicks = []
		for nick in plotData.plotdict["nicks"]:
			if nick not in tmp_nicks:
				tmp_nicks.append(nick)
		plotData.plotdict["nicks"] = tmp_nicks
		
