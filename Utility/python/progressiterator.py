
# -*- coding: utf-8 -*-

import math
import subprocess
import sys


class ProgressIterator(object):

	def __init__(self, iterable, length=None, description=""):
		self.iterator = iter(iterable)
		self.len = length if length != None else len(iterable)
		self.description = (description if description == "" else (description+"... "))
		self.current_index = 0
		self.current_progress = -1
		self.tty_width = int(subprocess.Popen("stty size", stdout=subprocess.PIPE, shell=True).communicate()[0].split()[-1])

	def __iter__(self):
		return self
	
	def next(self):
		current_progress = min(int(math.ceil(float(self.tty_width) * self.current_index / self.len)),
		                       self.tty_width)
		if current_progress > self.current_progress:
			self.current_progress = current_progress
			ratio = float(self.current_index) / self.len
			line = " %s%.1f %s" % (self.description, ratio*100, "%")
			line += (" " * (self.tty_width - len(line)))
			line = "\r\033[0;42m%s\033[0;41m%s\033[0m" % (line[:self.current_progress], line[self.current_progress:])
			sys.stdout.write(line)
			sys.stdout.flush()
		if self.current_index == self.len:
			sys.stdout.write("\n")
		self.current_index += 1
		return self.iterator.next()

