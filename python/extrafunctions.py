# -*- coding: utf-8 -*-

"""
Collection of functions to make plotting life faster
"""

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import os

import ROOT


def print_quantities(root_files=[""], root_folders=[""]):
	"""This functions prints a list of histograms or quantites present in one 
	folder or NTuple in the input files.
	"""
	for root_file_name, root_folder in zip(root_files, root_folders):
		quantities = {}
		if isinstance(root_file_name, list):
			root_file_name = root_file_name[0]
		if isinstance(root_folder, list):
			root_folder = root_folder[0]

		root_file = ROOT.TFile(root_file_name, "READ")
		obj = root_file.Get(root_folder)


		if (type(obj) == ROOT.TDirectoryFile):
			string = "Root histograms"
			func = "GetListOfKeys"
		elif (type(obj) == ROOT.TTree) or (type(obj) == ROOT.TNtuple):
			string = "Ntuple-variables"
			func = "GetListOfBranches"
		else:
			log.warning("Cannot access folder with name '%s'!" % root_folder)
			log.info("Available folders are:")
			for i in root_file.GetListOfKeys():
				log.info("\t" + i.GetName())
			return

		name = root_folder
		quantities[name] = []
		# Get the list of quantities from the object
		for branch in getattr(obj, func)():
			quantities[name] += [branch.GetTitle()]
		quantities[name] = set(quantities[name])

	if len(quantities) == 0:
		log.warning("Could not determine any quantites in this file.")
		return

	# Print the list of quantities present in ALL objects
	common_set = quantities[quantities.keys()[0]]
	for name in quantities.keys()[1:]:
		common_set = common_set.intersection(quantities[name])
	log.info('%s in ALL files:' % string)
	for q in sorted(common_set, key=lambda v: (v.upper(), v[0].islower())):
		log.info("  %s" % q)

	# Print the list of quantities that are present only in specific files
	for name in quantities.keys():
		quantities[name] = quantities[name].difference(common_set)
		if len(quantities[name]) > 0:
			log.info("Quantities only in '%s' file:" % name)
			for q in sorted(quantities[name], key=lambda v: (v.upper(), v[0].islower())):
				log.info("  %s" % q)

