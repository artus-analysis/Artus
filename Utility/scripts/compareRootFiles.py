#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT
import sys

opt_all = False

"""ROOT file compare tool
Usage: %s file1.root file2.root [-a]
2 files are required!
-a checks for all differences (otherwise the tool stops at the first difference)
"""

def main():
	global opt_all
	if len(sys.argv) < 3:
		log.info(__doc__)
		exit(2)
	inputFile1 = ROOT.TFile(sys.argv[1])
	inputFile2 = ROOT.TFile(sys.argv[2])
	opt_all = (len(sys.argv) > 3 and sys.argv[3] == "-a")
	if compareTree(inputFile1, inputFile2):
		log.info("The files \"" + inputFile1.GetName() + "\" and \"" + inputFile2.GetName(), "\" are identical.")
		exit(0)
	else:
		log.warning("The files \"" + inputFile1.GetName() + "\" and \"" + inputFile2.GetName(), "\" are NOT identical.")
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
		if obj.IsA().GetName().startswith("TH1") or obj.IsA().GetName().startswith("TProfile"):
			identified = True
			if not compareTH1(directory1, directory2, i.GetName()):
				log.info("problem with %s in %s" % (i.GetName(), directory1.GetPath().split(":")[1]))
				if not opt_all:
					return False
				result = False
		if obj.IsA().GetName().startswith("TH2"):
			identified = True
			if not compareTH2(directory1, directory2, i.GetName()):
				log.info("problem with %s in %s" % (i.GetName(), directory1.GetPath().split(":")[1]))
				if not opt_all:
					return False
				result = False
		if obj.IsA().GetName() == "TGraphErrors":
			identified = True
			if not compareGraph(directory1, directory2, i.GetName()):
				if not opt_all:
					return False
				result = False
		if obj.IsA().GetName() == "TNtuple":
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
		log.info("different number of leaves")
		if not opt_all:
			return False
		result = False
	for i in range(nleaves1):
		if leaves1.UncheckedAt(i).GetName() != leaves2.UncheckedAt(i).GetName():
			log.info("different leaf name: " + leaves1.UncheckedAt(i).GetName() + ", " + leaves2.UncheckedAt(i).GetName())
			if not opt_all:
				return False
			result = False
	if nevts1 != nevts2:
		log.info("different number of events")
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
			if leaves1.UncheckedAt(i).GetValue() != leaves2.UncheckedAt(i).GetValue():
				log.info("different leaf value: " + leaves1.UncheckedAt(i).GetValue() + ", " + leaves2.UncheckedAt(i).GetValue() + " for name " + leaves1.UncheckedAt(i).GetName())
				if not opt_all:
					return False
				result = False
	return result

if __name__ == "__main__":
	main()
