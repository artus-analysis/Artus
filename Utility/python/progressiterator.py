
# -*- coding: utf-8 -*-

import Artus.Utility.tools as tools

import math
import subprocess
import sys


class ProgressIterator(object):

	def __init__(self, iterable, length=None, description=""):
		self.iterator = iter(iterable)
		self.len = length if length != None else len(iterable)
		self.description = (description if description == "" else (description+"... "))
		self.current_index = 0
		self.ratio = -1.0
		self.tty_width = tools.get_tty_size()[1]

	def __iter__(self):
		return self
	
	def next(self):
		ratio = float(self.current_index) / self.len
		
		if int(ratio*100) > int(self.ratio*100):
			self.ratio = ratio
			current_progress = min(int(math.ceil(float(self.tty_width) * self.current_index / self.len)),
		                       self.tty_width)
			
			line = "%s%.1f %s" % (self.description, ratio*100, "%")
			#line += (" " * (self.tty_width - len(line)))
			line = line.center(self.tty_width)
			line = "\r\033[0;42m%s\033[0;41m%s\033[0m" % (line[:current_progress], line[current_progress:])
			
			sys.stdout.write(line)
			sys.stdout.flush()
		
		if self.current_index == self.len:
			sys.stdout.write("\r\033[J")
			sys.stdout.flush()
		
		self.current_index += 1
		try:
			return self.iterator.next()
		except StopIteration, e:
			sys.stdout.write("\r\033[J")
			sys.stdout.flush()
			raise StopIteration(e)

