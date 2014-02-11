#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""ArtusConfigBase provides the tools to make a valid artus config.

The most used functions are:
  - BaseConfig to generate a default configuration
  - CreateFileList to create a list of input files
  - Run to acutally call artus and run it
"""
import copy
import glob
import socket
import ConfigParser
import os
import stat
import getpass
import json


def FindFileInList(conf, number):
	for f in conf['InputFiles']:
		if "_" + str(number) in f:
			print conf['InputFiles'].index(f)


def getPath(variable=None, nofail=False):
	try:
		if not variable:
			path = os.environ["CMSSW_BASE"] + "/src/Artus/"
		else:
			path = os.environ[variable]
		return path
	except:
		print variable, "is not in shell variables:", os.environ.keys()
		print "Please source ini script and CMSSW!"
		if nofail:
			return None
		exit(1)


def addCHS(algorithms):
	"""can be used as [algos =] addCHS(algos) or algos = addCHS([AK5, etc.])"""
	algorithms += [a.replace("PFJets", "PFJetsCHS") for a in algorithms
		if "PFJets" in a and "PFJetsCHS" not in a and a.replace("PFJets", "PFJetsCHS") not in algorithms]
	return algorithms


#TODO: the next 3 functions must be cut down to analysis independent settings
# All options needed by common producers should be there, using reasonable defaults
# i.e.: whenever evry analysis needs this default or nothing bad happens: set it e.g. Quantities 
# whenever it is debatabel: use a default that will cause failure (e.g. InputFiles = [])
# alternative: no settings here, and defaults only in artus code
# use a more complex structure by grouping settings to their producers, cuts, consumers, filters?

def BaseConfig(inputtype, run='2012', analysis='z'):
	"""Basic configuration for Artus.

	Return a default configuration for Artus depending on

	  - @param inputtype can be either 'data' or 'mc'. Default settings are adapted.
	  - @param run can be either '2011' or '2012'. Parameters are set accordingly.
	  - @param analysis can be either 'zjet' or 'vbf'. The cuts are set accordingly.
	"""
	config = {
		'SkipEvents': 0,
		'EventCount': -1,
		'GlobalProducers': [],
		'L1Correction': 'L1FastJet',
		'Jec': "data or mc?",
		'JsonFile': "year?",
		'InputType': inputtype,
		'InputFiles': [],     # overridden by artus
		'OutputPath': "out",  # overridden by artus
		'MuonID2011': (run == '2011'),
		'Pipelines': {
			'default': {
				'LocalProducers': [],
				'Filters': [],
				'Cuts': [],
				'JetAlgorithm': "AK5PFJetsCHSL1L2L3",
				'Consumer': ["ntuple"],
				'QuantitiesVector': ["npv"],
			}
		},
		'PileupWeights': getPath() + "weights_190456-208686_8TeV_22Jan2013ReReco_68_5mb_kappa539_MC12_madgraph_tags.root",
		'PileupTruth': getPath() + "/data/pileup/pumean_pixelcorr.txt",
		'MinBiasXS': 68.5,
		'VetoPileupJets': False,
	}
	# any analysis must do this in its own framework:
	#config['Pipelines']['default'].update(GetCuts(analysis))

	if inputtype == 'data':
		config = SetDataSpecific(config, run)
	elif inputtype == 'mc':
		config = SetMcSpecific(config, run)
	else:
		print "The inputtype must be either 'data' or 'mc'."
		exit(1)

	return config


def SetMcSpecific(cfg, run='2012'):
	"""Add Monte-Carlo specific settings to a config.

	The MC settings include

	  - the set of Jet Energy Corrections
	  - pile-up reweighting factors
	  - additional producers
	"""
	if run == '2011':
		cfg['Jec'] = getPath() + "/data/jec/START44_V12"
		cfg["MuonSmearing"] = True
		cfg["MuonRadiationCorrection"] = False
		cfg["MuonCorrectionParameters"] = getPath() + "/data/muoncorrection/MuScleFit_2011_MC_44X.txt"
	elif run == '2012':
		cfg['Jec'] = getPath() + "/data/jec/Summer13_V5_MC"
		cfg["MuonSmearing"] = True
		cfg["MuonRadiationCorrection"] = False
		cfg["MuonCorrectionParameters"] = getPath() + "/data/muoncorrection/MuScleFit_2012_MC_53X_smearReReco.txt"
	else:
		print "MC period", run, "is undefined. No jet corrections known."
		exit(0)

	cfg['Pipelines']['default']['QuantitiesVector'] += ["npu"]
	cfg['GlobalProducers'] += ['gen_producer', 'pu_weight_producer', 'flavour_producer']
	cfg['NEvents'] = 30459503
	cfg['XSection'] = 3503.71
	return cfg


def SetDataSpecific(cfg, run='2012'):
	"""Add data specific settings to a config

	The data settings include

	  - the set of Jet Energy Corrections
	  - json files
	  - HLT paths
	  - additional producers
	"""
	d = {}
	if run == '2011':
		cfg['Jec'] = getPath() + "/data/jec/GR_R_44_V13"
		cfg['JsonFile'] = getPath() + "/data/json/Cert_160404-180252_7TeV_ReRecoNov08_Collisions11_JSON_v2.txt"
		cfg['HltPaths'] = ["HLT_DoubleMu7_v1"]
		cfg["MuonSmearing"] = False
		cfg["MuonRadiationCorrection"] = False
		cfg["MuonCorrectionParameters"] = getPath() + "data/muoncorrection/MuScleFit_2011_DATA_44X.txt"
	elif run == '2012':
		cfg['Jec'] = getPath() + "/data/jec/Summer13_V5_DATA"
		cfg['JsonFile'] = getPath() + "/data/json/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt"
		cfg['HltPaths'] = ["HLT_Mu17_Mu8_v%d" % v for v in range(1, 30)]
		cfg["MuonRadiationCorrection"] = False
		cfg["MuonSmearing"] = False
		cfg["MuonCorrectionParameters"] = getPath() + "/data/muoncorrection/MuScleFit_2012ABC_DATA_ReReco_53X.txt"
		cfg["MuonCorrectionParametersRunD"] = getPath() + "/data/muoncorrection/MuScleFit_2012D_DATA_ReReco_53X.txt"
	else:
		print "Run period", run, "is undefined. No json and jet corrections known."
		exit(1)

	cfg['Pipelines']['default']['QuantitiesVector'] += ['eventnr', 'lumisec']
	cfg['Pipelines']['default']['Filters'].append('json')
	cfg['Pipelines']['default']['Filters'].append('hlt')
	cfg['GlobalProducers'] += ['hlt_selector', 'pileuptruth_producer']
	return cfg


# TODO: to analysis
def GetCuts(analysis='z'):
	"""Return a set of default cuts for a given analysis."""
	cuts = {
		'z': {
			'GenCuts': False,
			'Cuts': [
				'zpt',
				'muon_eta',
				'muon_pt',
				'zmass_window',
			],
			'CutMuonEta': 2.3,
			'CutMuonPt': 20.0,
			'CutZMassWindow': 20.0,
			'CutLeadingJetEta': 1.3,

			'CutSecondLeadingToZPt': 0.2,
			'CutBack2Back': 0.34,

			'CutZPt': 30.0,
			'CutLeadingJetPt': 12.0,

			'Filters': ['valid_z', 'valid_jet', 'metfilter', 'incut'],
		},
	}
	if analysis not in cuts:
		print "There are no cuts defined for", analysis + "!"
		exit(1)
	return cuts[analysis]



# this function is partly analysis specific, but it shows how expansion could be used
def expand(config, variations=[], algorithms=[], default="default"):
	"""create pipelines for each algorithm times each variation"""
	pipelines = config['Pipelines']
	p = config['Pipelines'][default]
	if p['JetAlgorithm'] not in algorithms:
		algorithms.append(p['JetAlgorithm'])
	if config['InputType'] == 'data' and "Res" not in p['JetAlgorithm']:
		algorithms.append(p['JetAlgorithm'] + "Res")

	#find global algorithms
	config["GlobalAlgorithms"] = []
	removelist = ["Jets", "L1", "L2", "L3", "Res", "Hcal", "Custom"]
	for algo in algorithms:
		for r in removelist:
			algo = algo.replace(r, "").replace("CHS", "chs")
		if algo not in config["GlobalAlgorithms"]:
			config["GlobalAlgorithms"].append(algo)

	# copy for variations
	for v in variations:
		if v == 'all':
			pipelines[v] = copy.deepcopy(p)
			pipelines[v]['Cuts'] = []
			if 'incut' in pipelines[v]['Filters']:
					pipelines[v]['Filters'].remove('incut')
		if v == 'zcuts':
			pipelines[v] = copy.deepcopy(p)
			removelist = ['leadingjet_pt', 'back_to_back']
			for cut in removelist:
				if cut in pipelines[v]['Cuts']:
					pipelines[v]['Cuts'].remove(cut)
		if v == 'fullcuts':
			pipelines[v] = copy.deepcopy(p)
			pipelines[v]['Cuts'].append('leadingjet_eta')
			pipelines[v]['Cuts'].append('secondleading_to_zpt')
			pipelines[v]['CutLeadingJetEta'] = 1.3
			pipelines[v]['CutSecondLeadingToZPt'] = 0.2

	# rename template pipline default to incut
	pipelines['incut'] = pipelines.pop(default)

	# copy for algorithms, naming scheme: incut_algo
	for name, p in pipelines.items():
		for algo in algorithms:
			pipelines[name + "_" + algo] = copy.deepcopy(p)
			pipelines[name + "_" + algo]["JetAlgorithm"] = algo
		del pipelines[name]

	for p in pipelines.values():
		p['QuantitiesString'] = ":".join(p['QuantitiesVector'])

	return config


# this is generic and useful
def pipelinediff(config, to=None):
	print "Comparing", len(config['Pipelines']), "pipelines:"
	if to == None:
		to = filter(lambda x: 'incut' in x, config['Pipelines'].keys())[0]

	for name, p in config['Pipelines'].items():
		if name != to:
			print "- Compare", name, "to", to
			pipelinediff2(p, config['Pipelines'][to])
	print


def pipelinediff2(p1=None, p2=None):
	differencefound = False
	for k, v in p1.items():
		if k in p2.keys():
			if p1[k] != p2[k]:
				differencefound = True
				print "	different %s: %s != %s" % (k, str(p1[k]), str(p2[k]))
	if not differencefound:
		print "	identical"

