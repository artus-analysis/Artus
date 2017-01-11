
# -*- coding: utf-8 -*-

from Artus.Utility.tools import get_tty_size


import math
import subprocess
import sys


class ProgressIterator(object):

	def __init__(self, iterable, length=None, description="", visible=True):
		self.iterator = iter(iterable)
		self.len = length if length != None else len(iterable)
		self.description = (description if description == "" else (description+"... "))
		self.current_index = 0
		self.ratio = -1.0
		self.tty_width = get_tty_size()[1]
		self.visible = visible

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
			if self.visible:
				sys.stdout.write(line)
				sys.stdout.flush()
		
		if self.current_index == self.len and self.visible:
			sys.stdout.write("\r\033[J")
			sys.stdout.flush()
		
		self.current_index += 1
		try:
			return self.iterator.next()
		except StopIteration, e:
			sys.stdout.write("\r\033[J")
			sys.stdout.flush()
			raise StopIteration(e)

