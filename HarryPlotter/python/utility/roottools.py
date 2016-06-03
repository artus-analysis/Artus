# -*- coding: utf-8 -*-

"""
Collection of functions to ease handling ROOT histograms
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import array
import collections
import glob
import hashlib
import numpy
import os
import sys
import re
import tempfile

import ROOT
ROOT.gEnv.SetValue("TFile.AsyncPrefetching", 1)

import Artus.Utility.geometry as geometry
import Artus.Utility.tools as tools
from Artus.HarryPlotter.utility.tfilecontextmanager import TFileContextManager


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
		
		with TFileContextManager(root_file_names[0], "READ") as root_file: 
			root_object = root_file.Get(str(path_to_objects[0])) if str(path_to_objects[0]) != "" else root_file
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
							if key.GetClassName().startswith("TH") or key.GetClassName().startswith("TF") or key.GetClassName().startswith("Roo") or "Graph" in key.GetClassName():
								log.info("\t%s (%s)" % (path, key.GetClassName()))
					return ROOT.TDirectory
				else:
					log.error("Usage of ROOT objects of Type \"" + root_object.ClassName() + "\" is not yet implemented!")
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
			with TFileContextManager(root_file_name, "READ") as root_file:
				for path_to_histogram in path_to_histograms:
					tmp_root_histogram = root_file.Get(path_to_histogram)
					if tmp_root_histogram == None:
						log.critical("Cannot find histogram \"%s\" in file \"%s\"!" % (path_to_histogram, root_file_name))
						sys.exit(1)
				
					if tmp_root_histogram is None:
						log.error("Could not find histogram \"" + path_to_histogram + "\" in file \"" + root_file_name + "\"!")
					else:
						if isinstance(tmp_root_histogram, ROOT.TH1):
							tmp_root_histogram.SetDirectory(0)
					
						if root_histogram == None:
							root_histogram = tmp_root_histogram.Clone(name)
						else:
							root_histogram.Add(tmp_root_histogram)
					
						if isinstance(root_histogram, ROOT.TH1):
							root_histogram.SetDirectory(0)

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
		binning_identifier = "x".join([str(bins) for bins in [x_bins, y_bins, z_bins]])
	
		# prepare unique histogram name
		if name == None:
			name = "histogram_{0}".format(hashlib.md5("_".join([str(root_file_names),
				                                                str(path_to_trees),
				                                                variable_expression,
				                                                str(x_bins), str(y_bins), str(z_bins),
				                                                str(weight_selection)])).hexdigest())
		
		
		# prepare binning TTree::Draw/Project
		x_binning_string, y_binning_string, z_binning_string = ("",)*3

		if not binning_identifier in self.binning_determined:
			x_binning_string, self.x_bin_edges[binning_identifier] = RootTools.prepare_binning(x_bins)
			y_binning_string, self.y_bin_edges[binning_identifier] = RootTools.prepare_binning(y_bins)
			z_binning_string, self.z_bin_edges[binning_identifier] = RootTools.prepare_binning(z_bins)

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
			
			if any([bin_edges != None for bin_edges in [self.x_bin_edges[binning_identifier], self.y_bin_edges[binning_identifier], self.z_bin_edges[binning_identifier]]]):
				if any([bin_edges == None and expression != None for (bin_edges, expression) in zip(
						[self.x_bin_edges[binning_identifier], self.y_bin_edges[binning_identifier], self.z_bin_edges[binning_identifier]],
						[x_expression, y_expression, z_expression])]
				) and "prof" not in option.lower():
					log.warning("Bin edges need to be specified either for no or for all axes!")
				else:
					root_histogram = RootTools.create_root_histogram(
						x_bins=self.x_bin_edges[binning_identifier],
						y_bins=None if y_expression is None else self.y_bin_edges[binning_identifier],
						z_bins=None if z_expression is None else self.z_bin_edges[binning_identifier],
						profile_histogram=("prof" in option.lower()),
						name=name,
						profile_error_option=(option.lower().replace("prof", ''))
					)
		
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
		
		tree.SetName(str(hashlib.md5("".join(root_file_names))))
		
		# treat functions/macros that need to be compiled before drawing
		tmp_proxy_files = []
		proxy_call = None
		if "proxy" in option:
			proxy_name = re.sub("[^a-zA-Z0-9]", "_", variable_expression+"__"+str(weight_selection))
			
			proxy_class_name = "proxy_class_"+proxy_name
			proxy_macro_name = "proxy_macro_"+proxy_name
			proxy_cutmacro_name = "proxy_cutmacro_"+proxy_name
			
			proxy_class_filename = proxy_class_name+".h"
			proxy_macro_filename = proxy_macro_name+".C"
			proxy_cutmacro_filename = proxy_cutmacro_name+".C"
			
			tmp_proxy_files.append(proxy_class_filename)
			tmp_proxy_files.append(proxy_macro_filename)
			tmp_proxy_files.append(proxy_cutmacro_filename)
			
			# write macros with variable and cut expression containing the plotting information
			with open(proxy_macro_filename, "w") as proxy_macro_file:
				proxy_macro_file.write("double {function}() {a} return {content}; {b}".format(
						function=proxy_macro_name,
						a="{",
						content=variable_expression,
						b="}"
				))
			with open(proxy_cutmacro_filename, "w") as proxy_cutmacro_file:
				proxy_cutmacro_file.write("bool {function}() {a} return {content}; {b}".format(
						function=proxy_cutmacro_name,
						a="{",
						content=str(weight_selection),
						b="}"
				))
			
			# create tree proxy
			tree.MakeProxy(proxy_class_name, proxy_macro_filename, proxy_cutmacro_filename)
			proxy_call = proxy_class_filename+"+"
			
			# fix histogram name used in the proxy class
			# TODO: only do this when TTree::Project is called afterwards, not for TTree::Draw, when the histogram cannot be renamed before the plotting?
			proxy_class_content = None
			with open(proxy_class_filename) as proxy_class_file:
				proxy_class_content = proxy_class_file.read().rstrip("\n")
			proxy_class_content = proxy_class_content.replace(
					"if (htemp == 0)", "if (true)"
			).replace(
					"htemp = fDirector.CreateHistogram(GetOption());", "htemp = (TH1*)gDirectory->Get(\"{name}\");".format(name=name)
			).replace(
					"htemp->SetTitle", "//htemp->SetTitle"
			)
			with open(proxy_class_filename, "w") as proxy_class_file:
				proxy_class_file.write(proxy_class_content)
		
		# draw histogram
		if root_histogram == None:
			if ("proxy" in option) and (not proxy_call is None):
				log.critical("Plotting of compliled proxy formulas not yet implemented for the case where no binning is specified!")
				sys.exit(1)
			else:
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
					
					# histograms are sometimes empty, although the contain entries after the TH1::Print function is called
					# this is considered as a hack solving this problem.
					# https://root.cern.ch/doc/master/TH1_8cxx_source.html#l06558
					if isinstance(root_histogram, ROOT.TH1):
						root_histogram.GetSumOfWeights()
		else:
			if ("proxy" in option) and (not proxy_call is None):
				log.debug("TTree::Process(\"" + proxy_call + "\")")
				tree.Process(proxy_call)
			else:
				log.debug("TTree::Project(\"" + name + "\", \"" + variable_expression + "\", \"" + str(weight_selection) + "\", \"" + option + "\" GOFF\")")
				tree.Project(name, variable_expression, str(weight_selection), option + " GOFF")
			root_histogram = ROOT.gDirectory.Get(name)
		if root_histogram == None:
			log.critical("Cannot find histogram \"%s\" created from trees %s in files %s!" % (name, str(path_to_trees), str(root_file_names)))
			sys.exit(1)
		
		# delete possible files from tree proxy
		for tmp_proxy_file in tmp_proxy_files:
			for tmp_file in glob.glob(os.path.splitext(tmp_proxy_file)[0]+"*"):
				os.remove(tmp_file)
		
		if isinstance(root_histogram, ROOT.TH1):
			root_histogram.SetDirectory(0)
			self.x_bin_edges[binning_identifier] = RootTools.get_binning(root_histogram, axisNumber=0)
			self.y_bin_edges[binning_identifier] = RootTools.get_binning(root_histogram, axisNumber=1)
			self.z_bin_edges[binning_identifier] = RootTools.get_binning(root_histogram, axisNumber=2)
		elif isinstance(root_histogram, ROOT.TGraph):
			root_histogram.SetName(name)
			root_histogram.SetTitle("")

		if "prof" not in option.lower() and binning_identifier not in self.binning_determined:
			self.binning_determined.append(binning_identifier)
		return tree, root_histogram


	@staticmethod
	def create_root_histogram(x_bins, y_bins=None, z_bins=None, profile_histogram=False, name=None, profile_error_option=""):
		"""
		Create an empty ROOT histogram with a given binning
		"""
		# prepare unique histogram name
		if name == None:
			name = "histogram_{0}".format(hashlib.md5("_".join([str(x_bins), str(y_bins), str(z_bins), str(profile_histogram)])).hexdigest())
		
		# prepare bin edges
		x_bin_edges = None
		if isinstance(x_bins, basestring) or ((len(x_bins) == 1) and isinstance(x_bins[0], basestring)):
			x_binning_string, x_bin_edges = RootTools.prepare_binning(x_bins[0])
		else:
			x_bin_edges = array.array('d', x_bins)
		
		y_bin_edges = None
		if not y_bins is None:
			if isinstance(y_bins, basestring) or ((len(y_bins) == 1) and isinstance(y_bins[0], basestring)):
				y_binning_string, y_bin_edges = RootTools.prepare_binning(y_bins[0])
			else:
				y_bin_edges = y_bins
		
		z_bin_edges = None
		if not z_bins is None:
			if isinstance(z_bins, basestring) or ((len(z_bins) == 1) and isinstance(z_bins[0], basestring)):
				z_binning_string, z_bin_edges = RootTools.prepare_binning(z_bins[0])
			else:
				z_bin_edges = z_bins
		
		histogram_class = None
		histogram_class_name = None
		if not z_bins is None:
			histogram_class = ROOT.TProfile3D if profile_histogram else ROOT.TH3D
			histogram_class_name = "ROOT.TProfile3D" if profile_histogram else "ROOT.TH3D"
		elif not y_bins is None:
			histogram_class = ROOT.TProfile2D if profile_histogram else ROOT.TH2D
			histogram_class_name = "ROOT.TProfile2D" if profile_histogram else "ROOT.TH2D"
		else:
			histogram_class = ROOT.TProfile if profile_histogram else ROOT.TH1D
			histogram_class_name = "ROOT.TProfile" if profile_histogram else "ROOT.TH1D"
		
		histogram_args = [name, ""]
		histogram_args.extend([len(x_bin_edges)-1, x_bin_edges])
		if not y_bins is None:
			histogram_args.extend([len(y_bin_edges)-1, y_bin_edges])
			if not z_bins is None:
				histogram_args.extend([len(z_bin_edges)-1, z_bin_edges])
		if profile_histogram:
			histogram_args.append(profile_error_option)
		
		log.debug(histogram_class_name+"("+", ".join(["\""+arg+"\"" if isinstance(arg, basestring) else str(arg) for arg in histogram_args])+")")
		root_histogram = histogram_class(*histogram_args)
		
		return root_histogram

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
			tmp_root_histogram.RebinZ(simpleRebinning[2])
	
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
				log.debug("Old binning in X: " + RootTools.binning_formatted(binning))
				log.debug("New binning in X: " + RootTools.binning_formatted(complexRebinning[0]))
			sparse_tmp_root_histogram.GetAxis(0).Set(len(binning)-1, binning)
			if tmp_root_histogram.GetDimension() > 1:
				binning = RootTools.get_binning(tmp_root_histogram, 1)
				if not RootTools.rebinning_possible(binning, complexRebinning[1]):
					log.warning("Rebinning in Y leads to splitting/adding bins! Make sure you know what you are doing!")
					log.debug("Old binning in Y: " + RootTools.binning_formatted(binning))
					log.debug("New binning in Y: " + RootTools.binning_formatted(complexRebinning[1]))
				sparse_tmp_root_histogram.GetAxis(1).Set(len(binning)-1, binning)
			if tmp_root_histogram.GetDimension() > 2:
				binning = RootTools.get_binning(tmp_root_histogram, 2)
				if not RootTools.rebinning_possible(binning, complexRebinning[2]):
					log.warning("Rebinning in Z leads to splitting/adding bins! Make sure you know what you are doing!")
					log.debug("Old binning in Z: " + RootTools.binning_formatted(binning))
					log.debug("New binning in Z: " + RootTools.binning_formatted(complexRebinning[2]))
				sparse_tmp_root_histogram.GetAxis(2).Set(len(binning)-1, binning)
		
			# retrieve rebinned histogram
			sparse_rebinned_root_histogram.RebinnedAdd(sparse_tmp_root_histogram)
			if rebinned_root_histogram.GetDimension() > 2:
				rebinned_root_histogram = sparse_rebinned_root_histogram.Projection(0, 1, 2, "EO")
			elif rebinned_root_histogram.GetDimension() > 1:
				# non-intuitive swapping of the arguments: see https://root.cern.ch/root/html/THnSparse.html#THnSparse:Projection@1
				rebinned_root_histogram = sparse_rebinned_root_histogram.Projection(1, 0, "EO")
			else:
				rebinned_root_histogram = sparse_rebinned_root_histogram.Projection(0, "EO")
			
			rebinned_root_histogram.GetXaxis().Set(len(complexRebinning[0])-1, complexRebinning[0])
			rebinned_root_histogram.GetYaxis().Set(len(complexRebinning[1])-1, complexRebinning[1])
			rebinned_root_histogram.GetZaxis().Set(len(complexRebinning[2])-1, complexRebinning[2])
		
		# projections in case of only one bin for certain axes
		# TODO: this code might need a config option to be switched off by default
		if rebinned_root_histogram.GetDimension() == 2:
			if rebinned_root_histogram.GetNbinsY() == 1:
				rebinned_root_histogram = rebinned_root_histogram.ProjectionX("_px", 1, 1, "EO")
			elif rebinned_root_histogram.GetNbinsX() == 1:
				rebinned_root_histogram = rebinned_root_histogram.ProjectionY("_py", 1, 1, "EO")
		elif rebinned_root_histogram.GetDimension() == 3:
			if rebinned_root_histogram.GetNbinsZ() == 1:
				if rebinned_root_histogram.GetNbinsY() == 1:
					rebinned_root_histogram = rebinned_root_histogram.ProjectionX("_px", 1, 1, 1, 1, "EO")
				elif rebinned_root_histogram.GetNbinsX() == 1:
					rebinned_root_histogram = rebinned_root_histogram.ProjectionY("_py", 1, 1, 1, 1, "EO")
				else:
					rebinned_root_histogram.GetZaxis().SetRange(1, 1)
					rebinned_root_histogram = rebinned_root_histogram.Project3D("yx", "EO")
			elif rebinned_root_histogram.GetNbinsY() == 1:
				if rebinned_root_histogram.GetNbinsX() == 1:
					rebinned_root_histogram = rebinned_root_histogram.ProjectionZ("_pz", 1, 1, 1, 1, "EO")
				else:
					rebinned_root_histogram.GetYaxis().SetRange(1, 1)
					rebinned_root_histogram = rebinned_root_histogram.Project3D("zx", "EO")
			elif rebinned_root_histogram.GetNbinsX() == 1:
				rebinned_root_histogram.GetXaxis().SetRange(1, 1)
				rebinned_root_histogram = rebinned_root_histogram.Project3D("zy", "EO")
		
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
	def binning_formatted(binning):
		if len(set([str(upper-lower) for lower, upper in zip(binning[:-1], binning[1:])])) == 1:
			return "({n_bins}: [{lower}, {upper}])".format(
					n_bins=len(binning)-1,
					lower=binning[0],
					upper=binning[-1]
			)
		else:
			return "({n_bins}: {binning})".format(
					n_bins=len(binning)-1,
					binning=str(list(binning))
			)
	
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
		if isinstance(root_object, ROOT.TProfile2D):
			return root_object.ProjectionXY()
		elif isinstance(root_object, ROOT.TProfile):
			return root_object.ProjectionX()
		elif isinstance(root_object, ROOT.TH1):
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
				bin_edges = [(x_low+x_high)/2.0 for x_low, x_high in zip(x_values[:-1], x_values[1:])]
				bin_edges.insert(0, x_values[0] - ((bin_edges[0]-x_values[0]) / 2.0))
				bin_edges.append(x_values[-1] + ((x_values[-1]-bin_edges[-1]) / 2.0))
				if isinstance(root_object, ROOT.TGraphAsymmErrors) or isinstance(root_object, ROOT.TGraphErrors):
					bin_edges = RootTools.tgrapherr_get_binedges(root_object)
				
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
			root_directory = root_directory.Get(directory)
			root_directory.cd()
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
	
	@staticmethod
	def get_global_bins(root_histogram):
		assert isinstance(root_histogram, ROOT.TH1)
		global_bins = []
		for x_bin in xrange(1, root_histogram.GetNbinsX()+1):
			for y_bin in xrange(1, root_histogram.GetNbinsY()+1):
				for z_bin in xrange(1, root_histogram.GetNbinsZ()+1):
					global_bins.append(root_histogram.GetBin(x_bin, y_bin, z_bin))
		return global_bins

	@staticmethod
	def get_dimension(root_object):
		if isinstance(root_object, ROOT.TH1):
			return root_object.GetDimension()+1
		elif (isinstance(root_object, ROOT.TGraph) or isinstance(root_object, ROOT.TGraph2D)):
			return (3 if isinstance(root_object, ROOT.TGraph2D) else 2)
		elif isinstance(root_object, ROOT.TF1):
			if isinstance(root_object, ROOT.TF3):
				return 4
			elif isinstance(root_object, ROOT.TF2):
				return 3
			else:
				return 2
		else:
			log.warning("Retrieving the plot dimension is not yet implemented for objects of type %s!." % str(type(root_object)))
			return 2

	@staticmethod
	def get_min_max(root_object, axis=0, lower_threshold=None, upper_threshold=None):
		if isinstance(root_object, ROOT.TH1):
			if axis == 0:
				return root_object.GetXaxis().GetXmin(), root_object.GetXaxis().GetXmax()
			elif (axis == 1) and isinstance(root_object, ROOT.TH2):
				return root_object.GetYaxis().GetXmin(), root_object.GetYaxis().GetXmax()
			elif (axis == 2) and isinstance(root_object, ROOT.TH3):
				return root_object.GetZaxis().GetXmin(), root_object.GetZaxis().GetXmax()
			else:
				values = [root_object.GetBinContent(global_bin) for global_bin in RootTools.get_global_bins(root_object)]
				errors = [root_object.GetBinError(global_bin) for global_bin in RootTools.get_global_bins(root_object)]
				return RootTools.get_min(values, errors, lower_threshold), RootTools.get_max(values, errors, upper_threshold)
		
		elif (isinstance(root_object, ROOT.TGraph) or isinstance(root_object, ROOT.TGraph2D)):
			values = None
			errors_low = None
			errors_high = None
			if axis == 0:
				values = numpy.ndarray(root_object.GetN(), dtype=numpy.double, buffer=root_object.GetX())
				if isinstance(root_object, ROOT.TGraphAsymmErrors) or isinstance(root_object, ROOT.TGraphBentErrors):
					errors_low = numpy.ndarray(root_object.GetN(), dtype=numpy.double, buffer=root_object.GetEXlow())
					errors_high = numpy.ndarray(root_object.GetN(), dtype=numpy.double, buffer=root_object.GetEXhigh())
				elif isinstance(root_object, ROOT.TGraphErrors):
					errors_low = numpy.ndarray(root_object.GetN(), dtype=numpy.double, buffer=root_object.GetEX())
					errors_high = errors_low
				else:
					errors_low = [0.0] * len(values)
					errors_high = errors_low
			if axis == 1:
				values = numpy.ndarray(root_object.GetN(), dtype=numpy.double, buffer=root_object.GetY())
				if isinstance(root_object, ROOT.TGraphAsymmErrors) or isinstance(root_object, ROOT.TGraphBentErrors):
					errors_low = numpy.ndarray(root_object.GetN(), dtype=numpy.double, buffer=root_object.GetEYlow())
					errors_high = numpy.ndarray(root_object.GetN(), dtype=numpy.double, buffer=root_object.GetEYhigh())
				elif isinstance(root_object, ROOT.TGraphErrors):
					errors_low = numpy.ndarray(root_object.GetN(), dtype=numpy.double, buffer=root_object.GetEY())
					errors_high = errors_low
				else:
					errors_low = [0.0] * len(values)
					errors_high = errors_low
			if (axis == 2) and isinstance(root_object, ROOT.TGraph2D):
				values = numpy.ndarray(root_object.GetN(), dtype=numpy.double, buffer=root_object.GetZ())
				if isinstance(root_object, ROOT.TGraph2DErrors):
					errors_low = numpy.ndarray(root_object.GetN(), dtype=numpy.double, buffer=root_object.GetEZ())
					errors_high = errors_low
				else:
					errors_low = [0.0] * len(values)
					errors_high = errors_low
			if values is None:
				return None, None
			else:
				return RootTools.get_min(values, errors_low, lower_threshold), RootTools.get_max(values, errors_high, upper_threshold)
		
		elif isinstance(root_object, ROOT.TF1):
			if axis == 0:
				return root_object.GetXmin(), root_object.GetXmax()
			elif (axis == 1) and isinstance(root_object, ROOT.TF2):
				return root_object.GetYmin(), root_object.GetYmax()
			else:
				return root_object.GetMinimum(), root_object.GetMaximum()
		
		else:
			log.warning("Retrieving the plot limits is not yet implemented for objects of type %s!." % str(type(root_object)))
			return None, None
	
	@staticmethod
	def get_min(values, errors, lower_threshold=None):
		combined_values = [v-e for v, e in zip(values, errors)]
		if not lower_threshold is None:
			combined_values = [v for v in combined_values if v > lower_threshold]
		return (min(combined_values) if len(combined_values) > 0 else 0.)
	
	@staticmethod
	def get_max(values, errors, upper_threshold=None):
		combined_values = [v+e for v, e in zip(values, errors)]
		if not upper_threshold is None:
			combined_values = [v for v in combined_values if v < upper_threshold]
		return (max(combined_values) if len(combined_values) > 0 else 1.)
	
	@staticmethod
	def scale_tgraph(tgraph, scalefactor):
		N = tgraph.GetN()
		y = tgraph.GetY()
		for i in range(N):
			y[i] *= scalefactor
		if isinstance(tgraph, ROOT.TGraphAsymmErrors):
			yerrl = tgraph.GetEYlow()
			yerrh = tgraph.GetEYhigh()
			for i in range(N):
				yerrl[i] *= scalefactor
				yerrh[i] *= scalefactor
		elif isinstance(tgraph, ROOT.TGraphErrors):
			yerr = tgraph.GetEY()
			for i in range(N):
				yerr[i] *= scalefactor
		tgraph.GetHistogram().Delete()
		tgraph.SetHistogram(0)

	@staticmethod
	def tgraph_get_point(tgraph, i):
		tmpX, tmpY = ROOT.Double(0), ROOT.Double(0)
		tgraph.GetPoint(i, tmpX, tmpY)
		return float(tmpX), float(tmpY)

	@staticmethod
	def tgrapherr_get_binedges(tgraph):
		bin_edges = []
		for i in range(tgraph.GetN()):
			lowedge  = RootTools.tgraph_get_point(tgraph, i)[0] - tgraph.GetErrorXlow(i)
			highedge = RootTools.tgraph_get_point(tgraph, i)[0] + tgraph.GetErrorXhigh(i)
			if i == 0:
				bin_edges.append(lowedge)
			bin_edges.append(highedge)
		return bin_edges

	@staticmethod
	def load_compile_macro(macro):
		exit_code = ROOT.gROOT.LoadMacro(macro+"+")
		if exit_code != 0:
			macro_splitext = os.path.splitext(macro)
			macro_base = macro_splitext[0]+(macro_splitext[1].replace(".", "_"))
			tmp_files_to_remove = [macro_base+"."+ext for ext in ["d", "so"]]
			for tmp_file_to_remove in tmp_files_to_remove:
				if os.path.exists(tmp_file_to_remove):
					os.remove(tmp_file_to_remove)
			exit_code = ROOT.gROOT.LoadMacro(macro+"+")
		return exit_code

