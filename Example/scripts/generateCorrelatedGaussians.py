#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gErrorIgnoreLevel = ROOT.kError

import argparse
import numpy
import random

import Artus.Utility.tfilecontextmanager as tfilecontextmanager


if __name__ == "__main__":
	
	parser = argparse.ArgumentParser(description="Fill tree with (correlated) Gaussian random numbers", parents=[logger.loggingParser])
	
	parser.add_argument("--n-variables", type=int, default=5,
	                    help="Number of variables. [Default: %(default)s]")
	parser.add_argument("--n-events", type=int, default=10000,
	                    help="Number of events to generate. [Default: %(default)s]")
	parser.add_argument("-o", "--output", default="gaussians.root",
	                    help="Output filename. [Default: %(default)s]")
	
	args = parser.parse_args()
	logger.initLogger(args)
	
	variable_names = ["var"+str(index) for index in xrange(args.n_variables)]
	variables = [ROOT.RooRealVar(name, name, -100.0, 100.0) for name in variable_names]
	
	min_mean, max_mean = -5.0, 5.0
	means = ROOT.TVectorD(args.n_variables, numpy.array([float(random.randint(min_mean, max_mean)) for index in xrange(args.n_variables)]))
	log.debug("Mean values:")
	if log.isEnabledFor(logging.DEBUG):
		means.Print()
		
	min_sigma, max_sigma = 1.0, 5.0
	sigmas = [float(random.randint(min_sigma, max_sigma)) for index in xrange(args.n_variables)]
	
	min_correlation, max_correlation = -1.0, 1.0
	covariances = [[0 for index_1 in xrange(args.n_variables)] for index_2 in xrange(args.n_variables)]
	for index_1 in xrange(args.n_variables):
		for index_2 in xrange(index_1, args.n_variables):
			covariances[index_1][index_2] = sigmas[index_1] * sigmas[index_2]
			if index_1 != index_2:
				covariances[index_2][index_1] = sigmas[index_1] * sigmas[index_2]
				
				correlation = random.uniform(min_correlation, max_correlation)
				covariances[index_1][index_2] *= correlation
				covariances[index_2][index_1] *= correlation
	
	covariance = ROOT.TMatrixDSym(args.n_variables, numpy.array(covariances).flatten())
	log.debug("Covariance matrix:")
	if log.isEnabledFor(logging.DEBUG):
		covariance.Print()
	
	multidim_gaussian = ROOT.RooMultiVarGaussian(
			"multidim_gaussian",
			"multidim_gaussian",
			ROOT.RooArgList(*variables),
			means,
			covariance
	)
	
	ROOT.RooDataSet.setDefaultStorageType(ROOT.RooAbsData.Tree)
	mc_dataset = multidim_gaussian.generate(ROOT.RooArgSet(*variables), args.n_events)
	
	with tfilecontextmanager.TFileContextManager(args.output, "RECREATE") as root_file:
		mc_dataset.store().tree().Write("gaussians")
	log.info("Created tree \"%s\" in output file \"%s\"." % ("gaussians", args.output))

