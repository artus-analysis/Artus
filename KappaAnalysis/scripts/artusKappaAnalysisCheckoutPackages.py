#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import argparse

import Artus.KappaAnalysis.checkoutpackages as checkoutpackages


def main():

	parser = argparse.ArgumentParser(description="Checkout packages needed for Artus/KappaAnalysis.", parents=[logger.loggingParser])
	parser.add_argument("-n", "--n-trials", default=2, type=int,
	                    help="Maximum number of trials for checkout commands. [Default: %(default)s]")
	args = parser.parse_args()
	logger.initLogger(args)
	
	checkoutpackages.checkout_packages(max_n_trials=args.n_trials)


if __name__ == "__main__":
	main()

