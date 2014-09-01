#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import sys

import Artus.KappaAnalysis.tmvaclassification as tmvaclassification


if __name__ == "__main__" and len(sys.argv) > 1:
	tmvaclassification.tmva_classification()

