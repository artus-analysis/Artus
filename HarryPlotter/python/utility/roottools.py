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
import sys
import re

import ROOT
ROOT.gEnv.SetValue("TFile.AsyncPrefetching", 1)

import Artus.Utility.geometry as geometry
import Artus.Utility.tools as tools


class RootTools(object):
	def __init__(self):
		super(RootTools, self).__init__()
		self.binning_determined = []
		self.x_bin_edges = {}
		self.y_bin_edges = {}
		self.z_bin_edges = {}

	@staticmethod
	def check_type(root_file_names, path_to_objects, print_quantities=False):
		if isinstance(root_file_names, basestring):
			root_file_names = [root_file_names]
		if isinstance(path_to_objects, basestring):
			path_to_objects = [path_to_objects]
		
		root_file = ROOT.TFile(root_file_names[0], "READ")
		root_object = root_file.Get(path_to_objects[0]) if path_to_objects[0] != "" else root_file
		if root_object:
			if isinstance(root_object, ROOT.TTree):
				if print_quantities:
					log.info("List of all tree quantities (in the first file):")
					for leaf in sorted(root_object.GetListOfLeaves(), key=lambda leaf: leaf.GetName()):
						quantity = leaf.GetName()
						if leaf.GetBranch().GetMother().GetName() != leaf.GetName():
							quantity = leaf.GetBranch().GetMother().GetName()+"."+quantity
						log.info("\t%s (%s)" % (quantity, leaf.GetTypeName()))
				return ROOT.TTree
			elif isinstance(root_object, ROOT.TDirectory):
				if print_quantities:
					log.info("List of all histogram/graph/function quantities (in the first file):")
					for key, path in sorted(RootTools.walk_root_directory(root_object), key=lambda element: element[1]):
						if key.GetClassName().startswith("TH") or key.GetClassName().startswith("TF") or "Graph" in key.GetClassName():
							log.info("\t%s (%s)" % (path, key.GetClassName()))
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
				bin_edges = array.array(
						"d",
						[parsed_binnig[1] + (index*(parsed_binnig[2]-parsed_binnig[1])/parsed_binnig[0]) for index in xrange(int(parsed_binnig[0])+1)]
				)
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
	def histogram_from_file(root_file_names, path_to_histograms, x_bins=None, y_bins=None, z_bins=None, name=None):
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
					log.critical("Cannot find histogram \"%s\" in file \"%s\"!" % (path_to_histogram, root_file_name))
					sys.exit(1)
				
				tmp_root_histogram.SetDirectory(0)
				if tmp_root_histogram is None:
					log.error("Could not find histogram \"" + path_to_histogram + "\" in file \"" + root_file_name + "\"!")
				else:
					if root_histogram == None:
						root_histogram = tmp_root_histogram.Clone(name)
						root_histogram.SetDirectory(0)
					else:
						root_histogram.Add(tmp_root_histogram)
						root_histogram.SetDirectory(0)
			
			if root_file:
				root_file.Close()
			
		# rebinning
		if isinstance(root_histogram, ROOT.TH1) and root_histogram.GetNbinsX()*root_histogram.GetNbinsY()*root_histogram.GetNbinsZ() > 1:
			rebinning_x = 1
			if not x_bins is None:
				x_binning_string, x_bin_edges = RootTools.prepare_binning(x_bins)
				rebinning_x = x_bin_edges
				if x_bin_edges is None:
					rebinning_x = int(root_histogram.GetNbinsX() / int(x_bins[0]))
				else:
					rebinning_x = x_bin_edges
			
			rebinning_y = 1
			if not y_bins is None:
				y_binning_string, y_bin_edges = RootTools.prepare_binning(y_bins)
				rebinning_y = y_bin_edges
				if y_bin_edges is None:
					rebinning_y = int(root_histogram.GetNbinsY() / int(y_bins[0]))
				else:
					rebinning_y = y_bin_edges
			
			rebinning_z = 1
			if not z_bins is None:
				z_binning_string, z_bin_edges = RootTools.prepare_binning(z_bins)
				rebinning_z = z_bin_edges
				if z_bin_edges is None:
					rebinning_z = int(root_histogram.GetNbinsZ() / int(z_bins[0]))
				else:
					rebinning_z = z_bin_edges
			
			root_histogram = RootTools.rebin_root_histogram(
					root_histogram,
					rebinningX=rebinning_x,
					rebinningY=rebinning_y,
					rebinningZ=rebinning_z,
					name=name
			)
			
		return root_histogram


	def histogram_from_tree(self, root_file_names, path_to_trees,
		                    x_expression, y_expression=None, z_expression=None,
		                    x_bins=None, y_bins=None, z_bins=None,
		                    weight_selection="", option="", name=None, friend_trees=None):
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
		#determine histotype
		histo_type = "TH%.0f" % (1 + len(re.findall(":", variable_expression)))
	
		# prepare unique histogram name
		if name == None:
			name = "histogram_{0}".format(hashlib.md5("_".join([str(root_file_names),
				                                                str(path_to_trees),
				                                                variable_expression,
				                                                str(x_bins), str(y_bins), str(z_bins),
				                                                str(weight_selection)])).hexdigest())
		
		
		# prepare binning TTree::Draw/Project
		x_binning_string, y_binning_string, z_binning_string = ("",)*3

		if not histo_type in self.binning_determined:
			x_binning_string, self.x_bin_edges[histo_type] = RootTools.prepare_binning(x_bins)
			y_binning_string, self.y_bin_edges[histo_type] = RootTools.prepare_binning(y_bins)
			z_binning_string, self.z_bin_edges[histo_type] = RootTools.prepare_binning(z_bins)

		binning = ""
		root_histogram = None
		if not "TGraph" in option:
			if x_binning_string != None:
				binning += x_binning_string
			if y_binning_string != None and y_expression != None:
				binning += "," + y_binning_string
				if z_binning_string != None and z_expression != None:
					binning += "," + z_binning_string
			if not binning.startswith("("):
				binning = "(" + binning
			if not binning.endswith(")"):
				binning = binning + ")"
			if binning == "()":
				binning == ""
			
			if any([bin_edges != None for bin_edges in [self.x_bin_edges[histo_type], self.y_bin_edges[histo_type], self.z_bin_edges[histo_type]]]):
				if any([bin_edges == None and expression != None for (bin_edges, expression) in zip(
						[self.x_bin_edges[histo_type], self.y_bin_edges[histo_type], self.z_bin_edges[histo_type]],
						[x_expression, y_expression, z_expression])]
				) and "prof" not in option.lower():
					log.warning("Bin edges need to be specified either for no or for all axes!")
				else:
					if z_expression != None:
						if "prof" in option.lower():
							root_histogram = ROOT.TProfile2D(name, "",
							                                 len(self.x_bin_edges[histo_type])-1, self.x_bin_edges[histo_type],
							                                 len(self.y_bin_edges[histo_type])-1, self.y_bin_edges[histo_type])
							log.debug("TProfile2D(\"" + name + ", \"\", " +
							          str(len(self.x_bin_edges[histo_type])-1) + ", " + str(self.x_bin_edges[histo_type]) + ", " +
							          str(len(self.y_bin_edges[histo_type])-1) + ", " + str(self.y_bin_edges[histo_type]) + ")")
						else:
							root_histogram = ROOT.TH3D(name, "",
							                           len(self.x_bin_edges[histo_type])-1, self.x_bin_edges[histo_type],
							                           len(self.y_bin_edges[histo_type])-1, self.y_bin_edges[histo_type],
							                           len(self.z_bin_edges[histo_type])-1, self.z_bin_edges[histo_type])
							log.debug("TH3F(\"" + name + ", \"\", " +
							          str(len(self.x_bin_edges[histo_type])-1) + ", " + str(self.x_bin_edges[histo_type]) + ", " +
							          str(len(self.y_bin_edges[histo_type])-1) + ", " + str(self.y_bin_edges[histo_type]) + ", " +
							          str(len(self.z_bin_edges[histo_type])-1) + ", " + str(self.z_bin_edges[histo_type]) + ")")
					elif y_expression != None:
						if "prof" in option.lower():
							root_histogram = ROOT.TProfile(name, "",
							                               len(self.x_bin_edges[histo_type])-1, self.x_bin_edges[histo_type])
							log.debug("TProfile(\"" + name + ", \"\", " +
							          str(len(self.x_bin_edges[histo_type])-1) + ", " + str(self.x_bin_edges[histo_type]) + ")")
						else:
							root_histogram = ROOT.TH2D(name, "",
							                           len(self.x_bin_edges[histo_type])-1, self.x_bin_edges[histo_type],
							                           len(self.y_bin_edges[histo_type])-1, self.y_bin_edges[histo_type])
							log.debug("TH2F(\"" + name + ", \"\", " +
							          str(len(self.x_bin_edges[histo_type])-1) + ", " + str(self.x_bin_edges[histo_type]) + ", " +
							          str(len(self.y_bin_edges[histo_type])-1) + ", " + str(self.y_bin_edges[histo_type]) + ")")
					else:
						root_histogram = ROOT.TH1D(name, "",
						                           len(self.x_bin_edges[histo_type])-1, self.x_bin_edges[histo_type])
						log.debug("TH1F(\"" + name + ", \"\", " +
						          str(len(self.x_bin_edges[histo_type])-1) + ", " + str(self.x_bin_edges[histo_type]) + ")")
		
		# prepare TChain
		if isinstance(root_file_names, basestring):
			root_file_names = [root_file_names]
		if isinstance(path_to_trees, basestring):
			path_to_trees = [path_to_trees]
	
		tree = ROOT.TChain()
		for root_file_name in root_file_names:
			for path_to_tree in path_to_trees:
				complete_path_to_tree = os.path.join(root_file_name, path_to_tree)
				tree.Add(complete_path_to_tree)
				log.debug("Reading from ntuple %s ..." % complete_path_to_tree)
		
		#Add Friends
		if friend_trees is not None:
			for friend in friend_trees:
				tree.AddFriend(friend[0], friend[1])
		
		# ROOT optimisations
		tree.SetCacheSize(256*1024*1024) # 256 MB
		tree.AddBranchToCache("*", True)
		
		# draw histogram
		if root_histogram == None:
			draw_option = option.replace("TGraphAsymmErrorsX", "").replace("TGraphAsymmErrorsY", "").replace("TGraphErrors", "").replace("TGraph", "")
			log.debug("TTree::Draw(\"" + variable_expression + ">>" + name + binning + "\", \"" + str(weight_selection) + "\", \"" + draw_option + " GOFF\")")
			tree.Draw(variable_expression + ">>" + name + binning, str(weight_selection), draw_option + " GOFF")
			if "TGraphAsymmErrors" in option:
				n_points = tree.GetSelectedRows()
				x_values = tree.GetV2() if "TGraphAsymmErrorsX" in option else tree.GetV4()
				y_values = tree.GetV1()
				x_errors_low = tree.GetV3() if "TGraphAsymmErrorsX" in option else array.array("d", [0.0]*n_points)
				x_errors_high = tree.GetV4() if "TGraphAsymmErrorsX" in option else array.array("d", [0.0]*n_points)
				y_errors_low = array.array("d", [0.0]*n_points) if "TGraphAsymmErrorsX" in option else tree.GetV2()
				y_errors_high = array.array("d", [0.0]*n_points) if "TGraphAsymmErrorsX" in option else tree.GetV3()
				root_histogram = ROOT.TGraphAsymmErrors(n_points, x_values, y_values, x_errors_low, x_errors_high, y_errors_low, y_errors_high)
			elif "TGraphErrors" in option:
				root_histogram = ROOT.TGraphErrors(tree.GetSelectedRows(), tree.GetV3(), tree.GetV1(), tree.GetV4(), tree.GetV2())
			elif "TGraph" in option:
				root_histogram = ROOT.TGraph(tree.GetSelectedRows(), tree.GetV2(), tree.GetV1())
			else:
				root_histogram = ROOT.gDirectory.Get(name)
		else:
			log.debug("TTree::Project(\"" + name + "\", \"" + variable_expression + "\", \"" + str(weight_selection) + "\", \"" + option + "\" GOFF\")")
			tree.Project(name, variable_expression, str(weight_selection), option + " GOFF")
			root_histogram = ROOT.gDirectory.Get(name)
		if root_histogram == None:
			log.critical("Cannot find histogram \"%s\" created from trees %s in files %s!" % (name, str(path_to_trees), str(root_file_names)))
			sys.exit(1)
		
		if isinstance(root_histogram, ROOT.TH1):
			root_histogram.SetDirectory(0)
			self.x_bin_edges[histo_type] = RootTools.get_binning(root_histogram, axisNumber=0)
			self.y_bin_edges[histo_type] = RootTools.get_binning(root_histogram, axisNumber=1)
			self.z_bin_edges[histo_type] = RootTools.get_binning(root_histogram, axisNumber=2)
		elif isinstance(root_histogram, ROOT.TGraph):
			root_histogram.SetName(name)
			root_histogram.SetTitle("")

		if "prof" not in option.lower() and histo_type not in self.binning_determined:
			self.binning_determined.append(histo_type)
		return tree, root_histogram


	@staticmethod
	def rebin_root_histogram(root_histogram, rebinningX=1, rebinningY=1, rebinningZ=1, name=None):
		"""
		Rebin root histogram with well established root functions
	
		rebinningX/rebinningY/rebinningZ can either be of type int (number of neighbouring bins to merge into one bin)
		or lists of bin edges.
	
		The name (string) of the resulting histogram can be passed as a parameter
		"""

		# prepare unique histogram name
		if name == None:
			name = "histogram_{0}".format(hashlib.md5("_".join([root_histogram.GetName(),
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
			if not RootTools.rebinning_possible(binning, complexRebinning[0]):
				log.warning("Rebinning in X leads to splitting/adding bins! Make sure you know what you are doing!")
			sparse_tmp_root_histogram.GetAxis(0).Set(len(binning)-1, binning)
			if tmp_root_histogram.GetDimension() > 1:
				binning = RootTools.get_binning(tmp_root_histogram, 1)
				if not RootTools.rebinning_possible(binning, complexRebinning[1]):
					log.warning("Rebinning in Y leads to splitting/adding bins! Make sure you know what you are doing!")
				sparse_tmp_root_histogram.GetAxis(1).Set(len(binning)-1, binning)
			if tmp_root_histogram.GetDimension() > 2:
				binning = RootTools.get_binning(tmp_root_histogram, 2)
				if not RootTools.rebinning_possible(binning, complexRebinning[1]):
					log.warning("Rebinning in Z leads to splitting/adding bins! Make sure you know what you are doing!")
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
	def rebinning_possible(src_bin_edges, dst_bin_edges):
		if len(dst_bin_edges) > len(src_bin_edges):
			return False
		
		str_src_bin_edges = [str(bin_edge) for bin_edge in src_bin_edges]
		str_dst_bin_edges = [str(bin_edge) for bin_edge in dst_bin_edges]
		return set(str_dst_bin_edges).issubset(set(str_src_bin_edges))

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


	@staticmethod
	def to_histogram(root_object):
		if isinstance(root_object, ROOT.TH1):
			return root_object
		elif isinstance(root_object, ROOT.TGraph):
			if isinstance(root_object, ROOT.TGraph2D):
				log.warning("Conversion of objects of type %s into histograms is not yet implemented!" % str(type(root_object)))
				return root_object
			else:
				x_values = root_object.GetX()
				x_values = [x_values[index] for index in xrange(root_object.GetN())]
				
				# require x-values to be sorted
				# TODO: resorting the graph points currently not implemented
				assert all([x_low < x_high for x_low, x_high in zip(x_values[:-1], x_values[1:])])
				
				# determining the bin edges for the histogram
				# TODO: determining the bin edges currently only work properly for equidistant x-values
				bin_edges = [(x_low+x_high)/2.0 for x_low, x_high in zip(x_values[:-1], x_values[1:])]
				bin_edges.insert(0, x_values[0] - ((bin_edges[0]-x_values[0]) / 2.0))
				bin_edges.append(x_values[-1] + ((x_values[-1]-bin_edges[-1]) / 2.0))
				
				y_values = root_object.GetY()
				y_values = [y_values[index] for index in xrange(root_object.GetN())]
				
				y_errors = [0.0] * root_object.GetN()
				if isinstance(root_object, ROOT.TGraphAsymmErrors):
					y_errors_high = root_object.GetEYhigh()
					y_errors_low = root_object.GetEYlow()
					y_errors = [(y_errors_high[index]+y_errors_low[index])/2.0 for index in xrange(root_object.GetN())]
				elif isinstance(root_object, ROOT.TGraphErrors):
					y_errors = root_object.GetEY()
					y_errors = [y_errors[index] for index in xrange(root_object.GetN())]
				
				root_histogram = ROOT.TH1F("histogram_"+root_object.GetName(), root_object.GetTitle(), len(bin_edges)-1, array.array("d", bin_edges))
				for index, (y_value, y_error) in enumerate(zip(y_values, y_errors)):
					root_histogram.SetBinContent(index+1, y_value)
					root_histogram.SetBinError(index+1, y_error)
				return root_histogram
		else:
			log.warning("Conversion of objects of type %s into histograms is not yet implemented!" % str(type(root_object)))
			return root_object


	@staticmethod
	def merge_graphs(graphs, allow_reversed=False, allow_shuffle=False):
		"""
		Merge graphs
		"""
		if len(graphs) == 0:
			return None
		
		points = []
		for graph in graphs:
			x_values = graph.GetX()
			y_values = graph.GetY()
			points.append([[x_values[index], y_values[index]] for index in xrange(graph.GetN())])
		
		if allow_shuffle:
			points = geometry.order_lists_for_smallest_distances(points, allow_reversed=allow_reversed)
		
		merged_x_values, merged_y_values = zip(*tools.flattenList(points))
		merged_x_values = array.array("d", merged_x_values)
		merged_y_values = array.array("d", merged_y_values)
		graph = ROOT.TGraph(len(merged_x_values), merged_x_values, merged_y_values)
		return graph
	
	@staticmethod
	def write_object(root_file, root_object, path):
		root_file.cd()
		root_directory = root_file
		for directory in path.split("/")[:-1]:
			if root_directory.Get(directory) == None:
				root_directory.mkdir(directory)
			directory = root_directory.Get(directory)
			directory.cd()
		root_object.Write(path.split("/")[-1], ROOT.TObject.kWriteDelete)
		root_file.cd()
	
	@staticmethod
	def walk_root_directory(root_directory, path=""):
		elements = []
		for key in root_directory.GetListOfKeys():
			if key.GetClassName().startswith("TDirectory"):
				elements.extend(RootTools.walk_root_directory(root_directory.Get(key.GetName()),
					                                          os.path.join(path, key.GetName())))
			else:
				elements.append((key, os.path.join(path, key.GetName())))
		return elements

