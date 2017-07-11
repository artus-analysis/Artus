#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError

import sys

import Artus.HarryPlotter.utility.roottools as roottools

opt_all = False

relative_differences_per_leaf = {}

def main():
	global opt_all

	parser = argparse.ArgumentParser(description="ROOT file compare tool.",
	                                 parents=[logger.loggingParser])

	parser.add_argument("files", nargs=2,
	                    help="Two ROOT files to compare.")
	parser.add_argument("-a", "--all-differences", default=False, action="store_true",
	                    help="Check for all differences instead of stopping after the first difference. [Default: %(default)s]")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	inputFile1 = ROOT.TFile(args.files[0])
	inputFile2 = ROOT.TFile(args.files[1])
	opt_all = args.all_differences
	
	files_equal = compareTree(inputFile1, inputFile2)
	inputFile1.Close()
	inputFile2.Close()
	
	if files_equal:
		log.info("The files \"" + args.files[0] + "\" and \"" + args.files[1] + "\" are identical.")
		exit(0)
	else:
		log.warning("The files \"" + args.files[0] + "\" and \"" + args.files[1] + "\" are NOT identical.")
		if relative_differences_per_leaf != {}:
			for key in relative_differences_per_leaf.keys():
				log.warning("Average difference for {0}: {1:.2f}%".format(key, sum(relative_differences_per_leaf[key]) / len(relative_differences_per_leaf[key]) * 100.))
		exit(1)


def compareBin(histo1, histo2, idx):
	result = True
	if histo1.GetBinCenter(idx) != histo2.GetBinCenter(idx):
		log.info("center of bin %d not identical: %f vs %f" % (idx, histo1.GetBinCenter(idx), histo2.GetBinCenter(idx)))
		if not opt_all:
			return False
		result = False
	if histo1.GetBinWidth(idx) != histo2.GetBinWidth(idx):
		log.info("widths of bin %d not identical: %f vs %f" % (idx, histo1.GetBinWidth(idx), histo2.GetBinWidth(idx)))
		if not opt_all:
			return False
		result = False
	if histo1.GetBinError(idx) != histo2.GetBinError(idx):
		log.info("errors in bin %d not identical: %f vs %f" % (idx, histo1.GetBinError(idx), histo2.GetBinError(idx)))
		if not opt_all:
			return False
		result = False
	if histo1.GetBinContent(idx) != histo2.GetBinContent(idx):
		log.info("content of bin %d not identical: %f vs %f" % (idx, histo1.GetBinContent(idx), histo2.GetBinContent(idx)))
		if not opt_all:
			return False
		result = False
	return result


def compareTH1(directory1, directory2, histoID):
	result = True
	histo1 = directory1.Get(histoID)
	histo2 = directory2.Get(histoID)
	if histo1.GetNbinsX() != histo2.GetNbinsX():
		log.info("binning not identical")
		if not opt_all:
			return False
		result = False
	for binx in range(0, histo1.GetNbinsX() + 2):
		idx = histo1.GetBin(binx)
		if not compareBin(histo1, histo2, idx):
			if not opt_all:
				return False
			result = False
	return result


def compareTH2(directory1, directory2, histoID):
	histo1 = directory1.Get(histoID)
	histo2 = directory2.Get(histoID)
	if histo1.GetNbinsX() != histo2.GetNbinsX() or histo1.GetNbinsY() != histo2.GetNbinsY():
		log.info("binning not identical")
		return False
	try:
		for binx in range(0, histo1.GetNbinsX() + 2):
			for biny in range(0, histo1.GetNbinsY() + 2):
				idx1 = histo1.GetBin(binx, biny)
				idx2 = histo2.GetBin(binx, biny)
				if idx1 != idx2:
					log.info("binning not identical")
					return False
				if not compareBin(histo1, histo2, idx1):
					return False
	except:
		log.info("general error")
		return False
	return True


def compareTH3(directory1, directory2, histoID):
	histo1 = directory1.Get(histoID)
	histo2 = directory2.Get(histoID)
	if histo1.GetNbinsX() != histo2.GetNbinsX() or histo1.GetNbinsY() != histo2.GetNbinsY() or histo1.GetNbinsZ() != histo2.GetNbinsZ():
		log.info("binning not identical")
		return False
	try:
		for binx in range(0, histo1.GetNbinsX() + 2):
			for biny in range(0, histo1.GetNbinsY() + 2):
				for binz in range(0, histo1.GetNbinsZ() + 2):
					idx1 = histo1.GetBin(binx, biny, binz)
					idx2 = histo2.GetBin(binx, biny, binz)
					if idx1 != idx2:
						log.info("binning not identical")
						return False
					if not compareBin(histo1, histo2, idx1):
						return False
	except:
		log.info("general error")
		return False
	return True


def compareGraph(directory1, directory2, histoID):
	result = True
	histo1 = directory1.Get(histoID)
	histo2 = directory2.Get(histoID)
	if histo1.GetN() != histo2.GetN():
		log.info("number of points not identical")
		if not opt_all:
			return False
		result = False

	for point in range(histo1.GetN()):
		if not comparePoint(histo1, histo2, point):
			if not opt_all:
				return False
			result = False
	return result


def comparePoint(graph1, graph2, point):
	result = True
	x1 = ROOT.Double(+1.0)
	x2 = ROOT.Double(-1.0)
	y1 = ROOT.Double(+0.5)
	y2 = ROOT.Double(-0.5)
	graph1.GetPoint(point, x1, y1)
	graph2.GetPoint(point, x2, y2)
	if x1 != x2:
		log.info("x value of point %d in %s not identical: %f vs %f" % (point, graph1.GetName(), x1, x2))
		if not opt_all:
			return False
		result = False
	if y1 != y2:
		log.info("y value of point %d in %s not identical: %f vs %f" % (point, graph1.GetName(), y1, y2))
		if not opt_all:
			return False
		result = False
	x1 = graph1.GetErrorX(point)
	y1 = graph1.GetErrorY(point)
	x2 = graph2.GetErrorX(point)
	y2 = graph2.GetErrorY(point)
	if x1 != x2:
		log.info("x error of point %d in %s not identical: %f vs %f" % (point, graph1.GetName(), x1, x2))
		if not opt_all:
			return False
		result = False
	if y1 != y2:
		log.info("y error of point %d in %s not identical: %f vs %f" % (point, graph1.GetName(), y1, y2))
		if not opt_all:
			return False
		result = False
	return result


def compareTree(directory1, directory2):
	result = True
	for i in directory2.GetListOfKeys():
		obj = directory1.Get(i.GetName())
		obj2 = directory2.Get(i.GetName())
		if not obj:
			log.info("Object %s of type %s in %s is not available in first file" % (i.GetName(), obj2.IsA().GetName(), directory2.GetPath().split(":")[1]))
			if not opt_all:
				return False
			result = False
	for i in directory1.GetListOfKeys():
		obj = directory1.Get(i.GetName())
		obj2 = directory2.Get(i.GetName())
		if not obj2:
			log.info("Object %s of type %s in %s is not available in second file" % (i.GetName(), obj.IsA().GetName(), directory1.GetPath().split(":")[1]))
			if not opt_all:
				return False
			result = False
			continue
		identified = False
		if obj.IsA().GetName() == "TDirectoryFile":
			identified = True
			if not compareTree(directory1.Get(i.GetName()), directory2.Get(i.GetName())):
				if not opt_all:
					return False
				result = False
		elif isinstance(obj, ROOT.TH2):
			identified = True
			if not compareTH2(directory1, directory2, i.GetName()):
				log.info("problem with %s in %s" % (i.GetName(), directory1.GetPath().split(":")[1]))
				if not opt_all:
					return False
				result = False
		elif isinstance(obj, ROOT.TH1):
			identified = True
			if not compareTH1(directory1, directory2, i.GetName()):
				log.info("problem with %s in %s" % (i.GetName(), directory1.GetPath().split(":")[1]))
				if not opt_all:
					return False
				result = False
		elif isinstance(obj, ROOT.TGraphErrors):
			identified = True
			if not compareGraph(directory1, directory2, i.GetName()):
				if not opt_all:
					return False
				result = False
		elif isinstance(obj, ROOT.TTree):
			identified = True
			if not compareNtuple(directory1, directory2, i.GetName()):
				if not opt_all:
					return False
				result = False
		if not identified:
			log.warning("%s of type %s in %s has not been checked" % (i.GetName(), obj.IsA().GetName(), directory1.GetPath().split(":")[1]))
	return result


def compareNtuple(directory1, directory2, ntupleID):
	result = True
	ntuple1 = directory1.Get(ntupleID)
	ntuple2 = directory2.Get(ntupleID)
	ntuple1.GetEntry(2)
	ntuple2.GetEntry(2)
	leaves1 = ntuple1.GetListOfLeaves()
	leaves2 = ntuple2.GetListOfLeaves()
	nleaves1 = leaves1.GetEntriesFast()
	nleaves2 = leaves2.GetEntriesFast()
	nevts1 = ntuple1.GetEntries()
	nevts2 = ntuple1.GetEntries()
	if nleaves1 != nleaves2:
		only1 = [a.GetName() for a in leaves1 if not leaves2.FindObject(a.GetName())]
		only2 = [a.GetName() for a in leaves2 if not leaves1.FindObject(a.GetName())]
		log.critical("different number of leaves ("+ ntupleID + "):" + str(nleaves1) + " != " + str(nleaves2) + ".\n Only in root file 1: " + str(only1) + "\n Only in root file 2:" + str(only2) +"." )
		if not opt_all:
			return False
		result = False
	for i in range(nleaves1):
		leaf_name_1 = roottools.RootTools.full_leaf_name(leaves1.UncheckedAt(i))
		leaf_name_2 = roottools.RootTools.full_leaf_name(leaves2.UncheckedAt(i))
		if leaf_name_1 != leaf_name_2:
			log.critical("different leaf name: " + leaf_name_1 + ", " + leaf_name_2)
			if not opt_all:
				return False
			result = False
	if nevts1 != nevts2:
		log.critical("different number of events")
		if not opt_all:
			return False
		result = False
	nevts = min(nevts1, nevts2)
	for n in range(nevts):
		if n % 1000 == 0:
			log.info("\rEvent %d/%d (%1.1f%%)" % (n, nevts, n * 100. / nevts))
		ntuple1.GetEntry(n)
		ntuple2.GetEntry(n)
		for i in range(nleaves1):
			value2 = leaves2.UncheckedAt(i).GetValue() or 1e-9
			if (leaves1.UncheckedAt(i).GetValue() - leaves2.UncheckedAt(i).GetValue()) / value2 > 1e-6:
				log.critical("different leaf value: " + str(leaves1.UncheckedAt(i).GetValue()) + ", " + str(leaves2.UncheckedAt(i).GetValue()) + " for name " + roottools.RootTools.full_leaf_name(leaves1.UncheckedAt(i)))
				if leaves1.UncheckedAt(i).GetValue() != 0:
					# save relative difference between the two leaves
					if roottools.RootTools.full_leaf_name(leaves1.UncheckedAt(i)) not in relative_differences_per_leaf:
						relative_differences_per_leaf[roottools.RootTools.full_leaf_name(leaves1.UncheckedAt(i))] = []
					relative_differences_per_leaf[roottools.RootTools.full_leaf_name(leaves1.UncheckedAt(i))].append(abs(leaves1.UncheckedAt(i).GetValue() - leaves2.UncheckedAt(i).GetValue()) / leaves1.UncheckedAt(i).GetValue())
				if not opt_all:
					return False
				result = False
	return result

if __name__ == "__main__":
	main()
