#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from Artus.Configuration.artusWrapper import ArtusWrapper

if __name__ == "__main__":
	print "This is an example of how to use the artusWrapper."
	print "For illustrative purposes it just tries to print the JSON config file with cat."
	print
	
	wrapper = ArtusWrapper("cat")
	# User can manipulate config in code if desired
	conf = wrapper.getConfig()
	conf['higgs_mass'] = 126
	wrapper.setConfig(conf)

	wrapper.run()
