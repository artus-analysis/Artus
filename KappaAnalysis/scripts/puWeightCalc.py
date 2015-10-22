#!/usr/bin/env python
# -*- coding: utf-8 -*-
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True  # prevents Root from reading argv
import argparse
import subprocess
import copy
import array

"""This is a tool for pile-up reweighting.

   It requires: python, ROOT, CMSSW 5.2 (pileupCalc).
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
	mc = False
	data = False

	if not op.dfile and not op.mfile:
		print "Usage: weightCalc.py [options] datainput|mcinput [...]"
		print "Use -h for detailed help. This pileupCalc is used:"
		subprocess.call(['which', 'pileupCalc.py'])
		exit(0)

	if not op.mfile:
		print "No MC given."
	elif len(op.mfile) == 1:
		print "Get MC distribution from file", op.mfile[0]
		mc = getDistributionFromFile(op.mfile[0], op.mc_histo)
	else:
		print "Get MC distribution from skim"
		mc = getDistributionFromSkim(op.mfile)
		if op.save:
			saveDistributionToFile(op.mcoutput, mc, op.mc_histo)

	if not op.dfile:
		print "No data given."
	elif not op.inputLumiJSON:
		print "Get data distribution from file", op.dfile
		data = getDistributionFromFile(op.dfile, op.data_histo)
	else:
		print "Using:", subprocess.call(['pileupCalc.py'])
		data = getDataFromPileupCalc(op.dfile, op.inputLumiJSON, op.dataoutput,
				op.minBiasXsec, op.maxPileupBin, op.numPileupBins, histo="pileup")
		if op.save:
			saveDistributionToFile(op.dataoutput, data, op.data_histo)

	if data and mc:
		print data.GetNbinsX(), mc.GetNbinsX()
		if data.GetNbinsX() > 1000:
			data.Rebin()
		if mc.GetNbinsX() > 1000:
			mc.Rebin()
		weights = calcWeights(data, mc, op.verbose, not op.no_warning, op.rebin, op.binning)
		saveDistributionToFile(op.output, weights)
	else:
		print "No weights calculated."
#	if op.check:
#		print("Normalization: before ({0:.5f}) and after ({1:.5f}) ")


def calcWeights(data, mc, verbose=False, warn=True, rebin=False, binning=False):
	# checks
	n = data.GetNbinsX()
	if n != mc.GetNbinsX():
		print 'Binning of input histograms is not compatible.'
	print "Weight calculation ..."

	if rebin:
		print "rebin:", binning
		for l in binning:
			for a, b in zip(l[:-1], l[1:]):
				x1 = data.FindBin(a)
				x2 = data.FindBin(b)
				summc = sum([mc.GetBinContent(i) for i in range(x1, x2)]) / (x2 - x1)
				sumdata = sum([data.GetBinContent(i) for i in range(x1, x2)]) / (x2 - x1)
				for i in range(x1, x2):
					data.SetBinContent(i, sumdata)
					mc.SetBinContent(i, summc)

	# move after rebinning
	for i in range(1, n + 1):
		if warn and data.GetBinContent(i) <= 0 and data.GetBinLowEdge(i) < 59:
				print "WARNING: No data events with npu =", data.GetBinCenter(i)
		if warn and mc.GetBinContent(i) <= 0 and mc.GetBinLowEdge(i) < 59:
				print "WARNING: No MC events with npu =", mc.GetBinCenter(i)

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
	"""Get the true Data PU from the official PU-json

	   The "true" (or "observed") PU in data is estimated using this method:
	   https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData
	   1. pileupCalc is run (if json is given)
	   2. the distribution is read from a rootfile
	"""
	try:
		#fnull = open(os.devnull, 'w')
		#subprocess.call("pileupCalc.py", stdout=fnull, stderr=fnull)
		#fnull.close()
		pass
	except:
		print "pileupCalc.py is not availabe. Probably CMSSW is not sourced."
		exit(1)

	cmd = ["pileupCalc.py", "-i", files, "--inputLumiJSON", lumijson, "--calcMode", "true", "--minBiasXsec", str(int(minBiasXsec * 1000)), "--maxPileupBin", str(maxPileupBin), "--numPileupBins", str(numPileupBins), outfile]
	print " ".join(cmd)
	subprocess.call(cmd)

	result = getDistributionFromFile(outfile, histo)
	return result


def getDistributionFromSkim(filelist, nmax=80, save=True, bins=1600, name="pileup"):
	print "MC pile-up distribution from skim (" + str(len(filelist)) + " files):", filelist[0], "etc."
	chain = ROOT.TChain("Events")
	for f in filelist:
		chain.Add(f)
	print "Files added, starting to fill...",
	result = ROOT.TH1D(name, "True Number of Pile-Up Interactions;nputruth;events", bins, 0.0, nmax)
	chain.Draw("eventInfo.nPUMean >> " + name)
	print "done"
	return result


def saveDistributionToFile(filename, histo, histoname=None):
	rootfile = ROOT.TFile(filename, "RECREATE")
	if histoname:
		histo.SetName(histoname)
	histo.Write()
	rootfile.Close()
	print filename, "written"


def getDistributionFromFile(filename, histoname="pileup"):
	rootfile = ROOT.TFile(filename, "READONLY")
	if not rootfile.IsOpen():
		print "The file", filename, "could not be opened."
		exit(1)
	histo = rootfile.Get(histoname)
	if not histo:
		print "The histogram %s could not be found in %s." % (histoname, filename)
		exit(1)
	return copy.deepcopy(histo)


def options():
	parser = argparse.ArgumentParser(
		description="%(prog)s calculates the weights for MC reweighting "
			"according to the number of pile-up interactions. Use cases: "
			"%(prog)s /path/to/skim/*.root Cert_JSON.txt pileup_JSON.txt or"
			"%(prog)s mcdist.root datadist.root")
	parser.add_argument('files', metavar='input', type=str, nargs='*',
		help="input files, possible combinations:"
			"[data] [mc] where data can be a rootfile containing the estimated true number of pile-up "
			"interactions in data or the used json file and the official "
			"pile-up json. The name of the contained histogram is "
			"specified with -D. mc can be either a skim location or a rootfile with the MC distribution "
			"of pile-up. The name of the contained histogram is "
			"specified with -M.")
#Pileup calc options:
	parser.add_argument('-M', '--mc-histo', type=str, default="pileup",
		help="histogram name of the pile-up distribution in the MC file %(default)s")
	parser.add_argument('-D', '--data-histo', type=str, default="pileup",
		help="histogram name of the pile-up distribution in the data file %(default)s")
	parser.add_argument('-d', '--dataoutput', type=str, default="",
		help="Output file for the data distribution. Determined automatically "
			"if not specified.")
	parser.add_argument('-m', '--mcoutput', type=str, default="",
		help="Output file for the MC distribution. Determined automatically "
			"if not specified.")
	parser.add_argument('-o', '--output', type=str, default="data/pileup",
		help="Output directory/file for the calculated weights.")
	parser.add_argument('-s', '--save', action='store_true',
		help="Save data and MC distributions.")

	parser.add_argument('-i', '--inputLumiJSON', type=str, default=None,
		help="Input Lumi JSON for pileupCalc.")
	parser.add_argument('-x', '--minBiasXsec', type=float, default=69.4,  # 68.5 best value ever!
		help="Minimum bias cross section in mb (default: %(default)s mb, NB: pileupCalc takes ub!)")
	parser.add_argument('-n', '--maxPileupBin', type=int, default=80,
		help="Maximum number of pile-up interactions (default: %(default)s).")
	parser.add_argument('-b', '--numPileupBins', type=int, default=800,
		help="Number of bins (default: %(default)s).")
	parser.add_argument('-r', '--rebin', action='store_true',
		help="verbosity")

	parser.add_argument('-v', '--verbose', action='store_true',
		help="verbosity")
	parser.add_argument('-c', '--check', action='store_true',
		help="check whether the weights are correctly normalized. If no MC "
			"events are availabe for some numbers of pile-up interactions, "
			"the distribution can not be correctly normalized and the weights "
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

	# construct default names from input files
	# data: data_190456-208686_8TeV_22Jan2013ReReco.root
	if not args.dataoutput and args.inputLumiJSON:
		args.dataoutput = args.dfile.split('/')[-1]
		#print "I", args.dataoutput
		args.dataoutput = args.dataoutput[5:args.dataoutput.find("Collision") - 1]
		args.dataoutput = "data_" + args.dataoutput + ".root"
	if args.dfile and not args.dataoutput and not args.inputLumiJSON:
		args.dataoutput = args.dfile.split('/')[-1]

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
	args.binning = False
	if args.rebin:
		args.binning = [[0, 1, 2, 3.5, 5], range(45, args.maxPileupBin + 1)]

	if args.verbose:
		print args

	return args


if __name__ == "__main__":
	main()
