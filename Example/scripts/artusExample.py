#!/usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import sys
from Artus.Configuration.artusWrapper import ArtusWrapper


if __name__ == "__main__":

	artusWrapper = ArtusWrapper("artusExample")
	
	# User can manipulate config in code if desired
	conf = artusWrapper.getConfig()
	conf['higgs_mass'] = 126
	artusWrapper.setConfig(conf)

	# Run the wrapper
	sys.exit(artusWrapper.run())

