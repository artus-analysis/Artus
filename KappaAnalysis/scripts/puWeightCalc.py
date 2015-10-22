#!/usr/bin/env python
# -*- coding: utf-8 -*-
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True  # prevents Root from reading argv
import os
import argparse
import subprocess
import copy
import array
import sys

"""
This is a tool for reweighting MC to match pile-up in data.

It requires: python, ROOT, pileupCalc.py (CMSSW 5.2 or above).
It calculates the pu distribution in data with the official tool.
Then it calculates the pu distribution from a skim.
From these distributions it calculates the weights and saves them to a json
file. Data and MC distributions are saved to root files.
Pleas use --help or read the options function for further information.
"""
ROOT.gROOT.SetBatch(True)
ROOT.gErrorIgnoreLevel = ROOT.kError


def main():
	op = options()
	mc, data = None, None
	if not op.dfile and not op.mfile:
		print >> sys.stderr,  "No data nor MC files"
		print >> sys.stderr
		print >> sys.stderr,  "Usage: weightCalc.py [options] datainput|mcinput [...]"
		print >> sys.stderr,  "Use -h for detailed help. This pileupCalc is used:"
		subprocess.call(['which', 'pileupCalc.py'])
		sys.exit(1)
	# get pileup distribtions from input
	if not op.mfile:
		print >> sys.stderr,  "No MC given."
	elif len(op.mfile) == 1:
		print >> sys.stderr,  "Get MC distribution from file", op.mfile[0]
		mc = getDistributionFromFile(op.mfile[0], op.mc_histo)
	else:
		print >> sys.stderr,  "Get MC distribution from skim"
		mc = getDistributionFromSkim(op.mfile)
		if op.save:
			saveDistributionToFile(op.mcoutput, mc, op.mc_histo)
	if not op.dfile:
		print >> sys.stderr,  "No data given."
	elif not op.inputLumiJSON:
		print >> sys.stderr,  "Get data distribution from file", op.dfile
		data = getDistributionFromFile(op.dfile, op.data_histo)
	else:
		print >> sys.stderr,  "Get data distribution from pileupCalc.py"
		data = getDataFromPileupCalc(op.dfile, op.inputLumiJSON, op.dataoutput,
				op.minBiasXsec, op.maxPileupBin, op.numPileupBins, histo="pileup")
		if op.save:
			saveDistributionToFile(op.dataoutput, data, op.data_histo)
	if data and mc:
		print >> sys.stderr,  data.GetNbinsX(), mc.GetNbinsX()
		if data.GetNbinsX() > 1000:
			data.Rebin()
		if mc.GetNbinsX() > 1000:
			mc.Rebin()
		weights = calcWeights(data, mc, op.verbose, not op.no_warning, op.rebin, op.binning)
		saveDistributionToFile(op.output, weights)
	else:
		print >> sys.stderr,  "No weights calculated."
		sys.exit(1)


def calcWeights(data, mc, verbose=False, warn=True, rebin=False, binning=()):
	# checks
	n = data.GetNbinsX()
	if n != mc.GetNbinsX():
		print >> sys.stderr,  'Binning of input histograms is not compatible.'
	print >> sys.stderr,  "Weight calculation ..."

	if rebin:
		print >> sys.stderr,  "rebin:", binning
		for l in binning:
			for a, b in zip(l[:-1], l[1:]):
				x1 = data.FindBin(a)
				x2 = data.FindBin(b)
				summc = sum([mc.GetBinContent(i) for i in range(x1, x2)]) / (x2 - x1)
				sumdata = sum([data.GetBinContent(i) for i in range(x1, x2)]) / (x2 - x1)
				for i in range(x1, x2):
					data.SetBinContent(i, sumdata)
					mc.SetBinContent(i, summc)

	if warn:
		missing_data, missing_mc, missing_both = [], [], set()
		for i in range(1, n + 1):
			if data.GetBinContent(i) <= 0 and data.GetBinLowEdge(i) < 59:
				missing_data.append(data.GetBinCenter(i))
			if mc.GetBinContent(i) <= 0 and mc.GetBinLowEdge(i) < 59:
				missing_mc.append(mc.GetBinCenter(i))
		if missing_data and missing_mc:
			missing_both.update(missing_data)
			missing_both.intersection(missing_mc)
			print >> sys.stderr,  "WARNING: No data & mc events with npu =", sorted(missing_both)
		if missing_data:
			print >> sys.stderr,  "WARNING: No data events with npu =", [npu for npu in missing_data if not npu in missing_both]
		if missing_mc:
			print >> sys.stderr,  "WARNING: No mc events with npu =", [npu for npu in missing_mc if not npu in missing_both]

	# calculate
	data.Scale(1.0 / data.Integral())
	mc.Scale(1.0 / mc.Integral())
	weights = data.Clone('pileup')
	weights.SetTitle('pileup weights;n_{PU}^{truth};weight')
	weights.Divide(mc)

	for i in range(1, weights.GetNbinsX() + 1):
		if weights.GetBinContent(i) > 3:
			weights.SetBinContent(i, 3.0)
	return weights


def getDataFromPileupCalc(files, lumijson, outfile, minBiasXsec=73.5, maxPileupBin=80, numPileupBins=1600, histo="pileup"):
	"""
	Get the true Data PU from the official PU-json

	The "true" (or "observed") PU in data is estimated using this method:
	https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData
	1. pileupCalc is run (if json is given)
	2. the distribution is read from a rootfile
	"""
	cmd = ["pileupCalc.py", "-i", files, "--inputLumiJSON", lumijson, "--calcMode", "true", "--minBiasXsec", str(int(minBiasXsec * 1000)), "--maxPileupBin", str(maxPileupBin), "--numPileupBins", str(numPileupBins), outfile]
	print >> sys.stderr, "Using:", subprocess.check_output(['which', 'pileupCalc.py']).strip()
	print >> sys.stderr, " ".join(cmd)
	subprocess.call(cmd)
	result = getDistributionFromFile(outfile, histo)
	return result


def getDistributionFromSkim(filelist, nmax=80, save=True, bins=1600, histoname="pileup"):
	print >> sys.stderr, "MC pile-up distribution from skim: " + filelist[0] + ", ... (" + str(len(filelist)) + " files)"
	chain = ROOT.TChain("Events")
	for file_idx, file_name in enumerate(filelist):
		print >> sys.stderr, "Added %4d/%4d\r" % (file_idx + 1, len(filelist)),
		chain.Add(file_name)
	print >> sys.stderr, "Files added, starting to fill...",
	result = ROOT.TH1D(histoname, "True Number of Pile-Up Interactions;nputruth;events", bins, 0.0, nmax)
	chain.Draw("eventInfo.nPUMean >> " + histoname)
	print >> sys.stderr, "done"
	return result


def saveDistributionToFile(filename, histo, histoname=None):
	if not os.path.exists(os.path.dirname(filename)):
		print >> sys.stderr, "Creating output directory", os.path.dirname(filename)
		os.makedirs(os.path.dirname(filename))
	rootfile = ROOT.TFile(filename, "RECREATE")
	if histoname:
		histo.SetName(histoname)
	histo.Write()
	rootfile.Close()
	print >> sys.stderr, filename, "written"


def getDistributionFromFile(filename, histoname="pileup"):
	rootfile = ROOT.TFile(filename, "READONLY")
	if not rootfile.IsOpen():
		print >> sys.stderr, "The file", filename, "could not be opened."
		sys.exit(1)
	histo = rootfile.Get(histoname)
	if not histo:
		print >> sys.stderr, "The histogram %s could not be found in %s." % (histoname, filename)
		sys.exit(1)
	return copy.deepcopy(histo)


def options():
	parser = argparse.ArgumentParser(
		description="%(prog)s calculates the weights for MC reweighting "
			"according to the number of pile-up interactions. Use cases: "
			"%(prog)s /path/to/skim/*.root Cert_JSON.txt pileup_JSON.txt or"
			"%(prog)s mcdist.root datadist.root")
	parser.add_argument('files', metavar='input', type=str, nargs='*',
		help="""
		input files as [data] [mc, [mc, [...]]] - 'data' may be a root file
		with the distribution of pile-up interactions in data or a run json file
		(if a pile-up json is specified with '-i'). 'mc' may be a root file
		with the distribution of pile-up in MC or a set of skim files. For
		data/mc distribution files, see also '-M'/'-D'.
		""")
	#Pileup calc options:
	parser.add_argument('-M', '--mc-histo', type=str, default="pileup",
		help="histogram name of the pile-up distribution in the MC file %(default)s")
	parser.add_argument('-D', '--data-histo', type=str, default="pileup",
		help="histogram name of the pile-up distribution in the data file %(default)s")
	parser.add_argument('-d', '--dataoutput', type=str, default="",
		help="Output file for the data distribution. [Default: from data name]")
	parser.add_argument('-m', '--mcoutput', type=str, default="",
		help="Output file for the MC distribution. [Default: from mc name]")
	parser.add_argument('-o', '--output', type=str, default="data/pileup",
		help="Output path for the calculated weights.")
	parser.add_argument('-s', '--save', action='store_true',
		help="Save data and MC distributions.")
	parser.add_argument('-i', '--inputLumiJSON', type=str, default=None,
		help="Input Lumi JSON for pileupCalc.")
	parser.add_argument('-x', '--minBiasXsec', type=float, default=69.4,
		help="Minimum bias cross section in mb. [Default: %(default)s mb] NB: pileupCalc takes ub!")
	parser.add_argument('-n', '--maxPileupBin', type=int, default=80,
		help="Maximum number of pile-up interactions. [default: %(default)s]")
	parser.add_argument('-b', '--numPileupBins', type=int, default=800,
		help="Number of desired pile-up bins. [default: %(default)s].")
	parser.add_argument('-r', '--rebin', action='store_true',
		help="verbosity")
	parser.add_argument('-v', '--verbose', action='store_true',
		help="verbosity")
	parser.add_argument('-c', '--check', action='store_true',
		help="check whether the weights are correctly normalized. If no MC "
			"events are availabe for some numbers of pile-up interactions, "
			"the distribution cannot be correctly normalized and the weights "
			"do not exactly add up to unity.")
	parser.add_argument('-q', '--no-warning', action='store_true',
		help="Do not print warnings if the Monte Carlo sample does not "
			"contain events for all numbers of pile-up interactions.")
	parser.add_argument('-l', '--label', type=str, default="n_{{PU}}^{{truth}}",
		help="x-axis label in histograms")
	args = parser.parse_args()

	# distribute files to data (1 file) and mc (rest)
	args.dfile = None
	args.mfile = []
	if args.files:
		args.dfile = args.files[0]
		args.mfile = args.files[1:]
		if len(args.files) > 2 and args.files[0][:8] == args.files[1][:8]:  # assume: first is different -> data
			args.dfile = ""
			args.mfile = args.files
	if not args.dfile or not args.mfile:
		args.save = True

	# construct default names from (JSON) input files
	# data: data_190456-208686_8TeV_22Jan2013ReReco.root
	if not args.dataoutput and args.inputLumiJSON:
		args.dataoutput = os.path.basename(args.dfile)
		args.dataoutput = args.dataoutput[5:args.dataoutput.find("Collision") - 1]
		args.dataoutput = "data_" + args.dataoutput + ".root"
	if args.dfile and not args.dataoutput and not args.inputLumiJSON:
		args.dataoutput = os.path.basename(args.dfile)

	# mc: mc_kappa539_MC12_madgraph.root
	if not args.mcoutput and len(args.mfile) > 1:
		args.mcoutput = args.mfile[0].split('/')[-2]
		args.mcoutput = "mc_" + args.mcoutput + ".root"
	if not args.mcoutput and len(args.mfile) == 1:
		args.mcoutput = args.mfile[0].split('/')[-1]

	# weights: weights_190456-208686_8TeV_22Jan2013ReReco_kappa539_MC12_madgraph.root
	if args.output == 'data/pileup':
		args.output = args.output + '/' + args.dataoutput.replace(".root", "").replace("data_", "weights_") + '_' + args.mcoutput.replace("mc_", "")

	# if rebinning is activated, low stat bins are merged
	args.binning = []
	if args.rebin:
		args.binning = [[0, 1, 2, 3.5, 5], range(45, args.maxPileupBin + 1)]

	if args.verbose:
		print >> sys.stderr, "Using options:"
		print >> sys.stderr, args

	return args


if __name__ == "__main__":
	main()
