#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

for input_file in os.environ.get("FILE_NAMES", "").replace("\"", "").replace(",", "").split():
	command = "cp -v %s %s" % (input_file, os.path.basename(input_file))
	if input_file.startswith("root://"):
		command = "xrd" + command
	os.system(command)

