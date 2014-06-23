# -*- coding: utf-8 -*-

"""
Collection of functions to ease handling ROOT histograms
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array
import collections
import hashlib
import numpy
import os

import ROOT

class RootTools(object):
	def __init__(self):
		super(RootTools, self).__init__()
		self.binning_determined = False

	@staticmethod
	def check_type(root_file_names, path_to_objects):
		if isinstance(root_file_names, basestring):
			root_file_names = [root_file_names]
		if isinstance(path_to_objects, basestring):
			path_to_objects = [path_to_objects]
	
		if path_to_objects[0] == "":
			return ROOT.TH1
	
		root_file = ROOT.TFile(root_file_names[0], "READ")
		root_object = root_file.Get(path_to_objects[0])
		if root_object:
			if isinstance(root_object, ROOT.TTree):
				return ROOT.TTree
			elif isinstance(root_object, ROOT.TDirectory):
				return ROOT.TH1
			else:
				log.error("Usage of ROOT objects of Type \"" + root_objects[0].ClassName() + "\" is not yet implemented!")
				return None
		else:
			log.error("Could not find ROOT object \"" + path_to_objects[0] + "\" in file \"" + root_file_names[0] + "\"!")
			return None


	@staticmethod
	def binning_to_bin_edges(binning):
		"""
		returns (binning_string, bin_edges)
		"""
		binning_string = None
		bin_edges = None
		if isinstance(binning, basestring):
			binning_string = binning
			parsed_binnig = [float(item) for item in binning.split(",")[:3]]
			if len(parsed_binnig) == 3:
				bin_edges = array.array("d", list(numpy.arange(parsed_binnig[1],
				                                  parsed_binnig[2]+0.5*(parsed_binnig[2]-parsed_binnig[1])/parsed_binnig[0],
				                                  (parsed_binnig[2]-parsed_binnig[1])/parsed_binnig[0])))
			else:
				binning_string += ("," * (3 - len(parsed_binnig)))
		elif isinstance(binning, collections.Iterable):
			binning = [float(item) for item in binning]
			binning_string = "%d,%f,%f" % (len(binning), min(binning), max(binning))
			bin_edges = array.array("d", binning)
		return binning_string, bin_edges


	@staticmethod
	def prepare_binning(binning):
		"""
		returns (binning_string, bin_edges)
		"""
		if len(binning) == 1:
			binning = binning[0]
		
		prepared_binning = binning
		if prepared_binning == None:
			prepared_binning = ""
		if isinstance(prepared_binning, basestring):
			prepared_binning.replace("(", "")
			prepared_binning.replace(")", "")
			
		binning_string, bin_edges = RootTools.binning_to_bin_edges(prepared_binning)
	
		return binning_string, bin_edges


	@staticmethod
	def histogram_from_file(root_file_names, path_to_histograms, name=None):
		"""
		Read histograms from files
	
		root_file_names: string (or list of strings)
		path_to_histograms: string (or list of strings) of path to root histogram in root file
	
		This function looks for the same histograms in all files and sums them up
		The name (string) of the resulting histogram can be passed as a parameter
		"""

		if isinstance(root_file_names, basestring):
			root_file_names = [root_file_names]
		if isinstance(path_to_histograms, basestring):
			path_to_histograms = [path_to_histograms]
	
		# prepare unique histogram name
		if name == None:
			name = "histogram_{0}.json".format(hashlib.md5("_".join([str(root_file_names),
				                                                     str(path_to_histograms)])).hexdigest())
	
		# loop over files and try to read histograms
		root_histogram = None
		for root_file_name in root_file_names:
			root_file = ROOT.TFile(root_file_name, "READ")
		
			for path_to_histogram in path_to_histograms:
				tmp_root_histogram = root_file.Get(path_to_histogram)
				if tmp_root_histogram == None:
					log.error("Could not find histogram \"" + path_to_histogram + "\" in file \"" + root_file_name + "\"!")
				else:
					if root_histogram:
						root_histogram.Add(tmp_root_histogram)
					else:
						root_histogram = tmp_root_histogram
						root_histogram.SetName(name)
	
		root_histogram.SetDirectory(0)
		return root_histogram


	def histogram_from_tree(self, root_file_names, path_to_trees,
		                    x_expression, y_expression=None, z_expression=None,
		                    x_bins=None, y_bins=None, z_bins=None,
		                    weight_selection="", option="", name=None):
		"""
		Read histograms from trees
	
		root_file_names: string or list of strings
		path_to_tree: string of path to root tree in root file
	
		This will build up a TChain to read out all trees from all files
	
		variable_expression: Used as varexp parameter of TTree::Draw
		binning: string, can be empty, "<nbins>", "<nbins>, <low>, <up>", ...
		weight_selection: Used as cut parameter of TTree::Draw
		option: Used as option parameter of TTree::Draw, "GOFF" is added
	
		The name (string) of the resulting histogram can be passed as a parameter
		"""
	
		variable_expression = "%s%s%s" % (z_expression + ":" if z_expression else "",
			                              y_expression + ":" if y_expression else "",
			                              x_expression)
	
		# prepare unique histogram name
		if name == None:
			name = "histogram_{0}.json".format(hashlib.md5("_".join([str(root_file_names),
				                                                     str(path_to_trees),
				                                                     variable_expression,
				                                                     str(x_bins), str(y_bins), str(z_bins),
				                                                     str(weight_selection)])).hexdigest())
		
		
		# prepare binning TTree::Draw/Project
		x_binning_string = ""
		y_binning_string = ""
		z_binning_string = ""
		
		if not self.binning_determined:
			x_binning_string, self.x_bin_edges = RootTools.prepare_binning(x_bins)
			y_binning_string, self.y_bin_edges = RootTools.prepare_binning(y_bins)
			z_binning_string, self.z_bin_edges = RootTools.prepare_binning(z_bins)
		
		binning = ""
		if x_binning_string != None:
			binning += x_binning_string
		if y_binning_string != None and y_expression != None:
			binning += y_binning_string
			if z_binning_string != None and z_expression != None:
				binning += z_binning_string
		if not binning.startswith("("):
			binning = "(" + binning
		if not binning.endswith(")"):
			binning = binning + ")"
		if binning == "()":
			binning == ""
	
		root_histogram = None
		if any([bin_edges != None for bin_edges in [self.x_bin_edges, self.y_bin_edges, self.z_bin_edges]]):
			if any([bin_edges == None for bin_edges in [self.x_bin_edges, self.y_bin_edges, self.z_bin_edges]]):
				log.warning("Bin edges need to be specified either for no or for all axes!")
			else:
				if z_expression != None:
					root_histogram = ROOT.TH3F(name, "",
							                   len(self.x_bin_edges)-1, self.x_bin_edges,
							                   len(self.y_bin_edges)-1, self.y_bin_edges,
								               len(self.z_bin_edges)-1, self.z_bin_edges)
				elif y_expression != None:
					root_histogram = ROOT.TH2F(name, "",
							                   len(self.x_bin_edges)-1, self.x_bin_edges,
							                   len(self.y_bin_edges)-1, self.y_bin_edges)
				else:
					root_histogram = ROOT.TH1F(name, "",
							                   len(self.x_bin_edges)-1, self.x_bin_edges)
	
		# prepare TChain
		if isinstance(root_file_names, basestring):
			root_file_names = [root_file_names]
		if isinstance(path_to_trees, basestring):
			path_to_trees = [path_to_trees]
	
		tree = ROOT.TChain()
		for root_file_name in root_file_names:
			for path_to_tree in path_to_trees:
				tree.Add(os.path.join(root_file_name, path_to_tree))
	
		# draw histogram
		if root_histogram == None:
			tree.Draw(variable_expression + ">>" + name + binning, str(weight_selection), option + " GOFF")
		else:
			tree.Project(name, variable_expression, str(weight_selection), option + " GOFF")
		root_histogram = ROOT.gDirectory.Get(name)
		root_histogram.SetDirectory(0)
	
		self.x_bin_edges = RootTools.get_binning(root_histogram, axisNumber=0)
		self.y_bin_edges = RootTools.get_binning(root_histogram, axisNumber=1)
		self.z_bin_edges = RootTools.get_binning(root_histogram, axisNumber=2)
		self.binning_determined = True
	
		return root_histogram


	def rebin_root_histogram(self, root_histogram, rebinningX=1, rebinningY=1, rebinningZ=1, name=None):
		"""
		Rebin root histogram with well established root functions
	
		rebinningX/rebinningY/rebinningZ can either be of type int (number of neighbouring bins to merge into one bin)
		or lists of bin edges.
	
		The name (string) of the resulting histogram can be passed as a parameter
		"""

		# prepare unique histogram name
		if name == None:
			name = "histogram_{0}.json".format(hashlib.md5("_".join([root_histogram.GetName(),
				                                                     str(rebinningX),
				                                                     str(rebinningY),
				                                                     str(rebinningZ)])).hexdigest())

		simpleRebinning = {
			0 : rebinningX if type(rebinningX) == int and rebinningX != 1 else None,
			1 : rebinningY if type(rebinningY) == int and rebinningY != 1 else None,
			2 : rebinningZ if type(rebinningZ) == int and rebinningZ != 1 else None,
		}
		complexRebinning = {
			0 : array.array("d", list(rebinningX)) if type(rebinningX) != int else None,
			1 : array.array("d", list(rebinningY)) if type(rebinningY) != int else None,
			2 : array.array("d", list(rebinningZ)) if type(rebinningZ) != int else None,
		}
	
		# simple rebinning
		tmp_root_histogram = root_histogram.Clone(name+"clone")
		if simpleRebinning[0]:
			tmp_root_histogram.RebinX(simpleRebinning[0])
		if simpleRebinning[1] and tmp_root_histogram.GetDimension() > 1:
			tmp_root_histogram.RebinY(simpleRebinning[1])
		if simpleRebinning[2] and tmp_root_histogram.GetDimension() > 2:
			tmp_root_histogram.RebinY(simpleRebinning[2])
	
		# complex rebinning (non-constant bin widths)
		rebinned_root_histogram = tmp_root_histogram.Clone(name)
		if any(complexRebinning.values()):
	
			# create exmpty histogram with correct final binning
			rebinned_root_histogram.Reset()
		
			complexRebinning = { axisNumber : rebinning if rebinning else RootTools.get_binning(rebinned_root_histogram, axisNumber)
				                 for axisNumber, rebinning in complexRebinning.items() }
		
			rebinned_root_histogram.GetXaxis().Set(len(complexRebinning[0])-1, complexRebinning[0])
			rebinned_root_histogram.GetYaxis().Set(len(complexRebinning[1])-1, complexRebinning[1])
			rebinned_root_histogram.GetZaxis().Set(len(complexRebinning[2])-1, complexRebinning[2])
		
			# use THnSparse to correctly sum up histograms with different binnings
			sparse_rebinned_root_histogram = ROOT.THnSparse.CreateSparse(name+"sparse", "", rebinned_root_histogram)
			for axisNumber, rebinning in complexRebinning.items():
				if axisNumber < rebinned_root_histogram.GetDimension():
					sparse_rebinned_root_histogram.GetAxis(axisNumber).Set(len(rebinning)-1, rebinning)
		
			sparse_tmp_root_histogram = ROOT.THnSparse.CreateSparse(name+"sparsetmp", "", tmp_root_histogram)
			binning = RootTools.get_binning(tmp_root_histogram, 0)
			sparse_tmp_root_histogram.GetAxis(0).Set(len(binning)-1, binning)
			if tmp_root_histogram.GetDimension() > 1:
				binning = RootTools.get_binning(tmp_root_histogram, 1)
				sparse_tmp_root_histogram.GetAxis(1).Set(len(binning)-1, binning)
			if tmp_root_histogram.GetDimension() > 2:
				binning = RootTools.get_binning(tmp_root_histogram, 2)
				sparse_tmp_root_histogram.GetAxis(2).Set(len(binning)-1, binning)
		
			# retrieve rebinned histogram
			sparse_rebinned_root_histogram.RebinnedAdd(sparse_tmp_root_histogram)
			if rebinned_root_histogram.GetDimension() > 2:
				rebinned_root_histogram = sparse_rebinned_root_histogram.Projection(0, 1, 2)
			elif rebinned_root_histogram.GetDimension() > 1:
				rebinned_root_histogram = sparse_rebinned_root_histogram.Projection(0, 1)
			else:
				rebinned_root_histogram = sparse_rebinned_root_histogram.Projection(0)
		
			rebinned_root_histogram.GetXaxis().Set(len(complexRebinning[0])-1, complexRebinning[0])
			rebinned_root_histogram.GetYaxis().Set(len(complexRebinning[1])-1, complexRebinning[1])
			rebinned_root_histogram.GetZaxis().Set(len(complexRebinning[2])-1, complexRebinning[2])
		
			rebinned_root_histogram.SetName(name)
	
		return rebinned_root_histogram


	@staticmethod
	def get_binning(root_histogram, axisNumber=0):
		"""
		Returns the bin edges of a certain axis for a given root histogram
		"""
	
		axis = None
		if axisNumber == 0:
			axis = root_histogram.GetXaxis()
		elif axisNumber == 1:
			axis = root_histogram.GetYaxis()
		elif axisNumber == 2:
			axis = root_histogram.GetZaxis()
		return array.array("d", [axis.GetBinLowEdge(binIndex) for binIndex in xrange(1, axis.GetNbins()+2)])


	@staticmethod
	def add_root_histograms(*root_histograms, **kwargs):
		"""
		Sums up all histograms in the list of args.
		The returned histogram will have the name kwargs.get("name", args[0].GetName()+"_sum")
		"""
	
		histogram_sum = None
		for histogram in root_histograms:
			if histogram_sum == None:
				histogram_sum = histogram.Clone(kwargs.get("name", root_histograms[0].GetName()+"_sum"))
			else:
				histogram_sum.Add(histogram)
		return histogram_sum
	
