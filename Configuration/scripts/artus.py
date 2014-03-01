#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

import Artus.Configuration.artusWrapper as artusWrapper


if __name__ == "__main__":
	print "This is an example of how to use the artusWrapper."
	print "For illustrative purposes it just tries to print the JSON config file with cat."
	print
	
	sys.exit(artusWrapper.artusWrapper("cat"))

