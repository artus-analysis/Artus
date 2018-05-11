
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import copy
import re

"""
	This module contains a dictionary for expressions.
"""
class ExpressionsDict(object):
	def __init__(self, additional_expressions=None):
		self.expressions_dict = {}
		# mapping name => formula; example:
		# self.expressions_dict['deltaphijet1jet2'] = '(abs(abs(abs(jet1phi-jet2phi)-TMath::Pi())-TMath::Pi()))'
		self.expressions_regex = []
		# list[tuple] signature => formula; example:
		# self.expressions_regex.append(r"delta_phi\((\w+),(\w+)\)", r'(abs(abs(abs(\1phi-\2phi)-TMath::Pi())-TMath::Pi()))')

		if not additional_expressions is None:
			self.expressions_dict.update(additional_expressions)

	def get_expression(self, expression):
		return self.expressions_dict.get(expression.lower(), expression)

	def invert(self, expression):
		return "(!" + expression + ")"

	def combine(self, strings_to_combine):
		return "(" + "*".join(strings_to_combine) + ")"

	def replace_expressions(self, expression):
		"""Replace any known expressions"""
		new_expr = copy.deepcopy(expression)
		if new_expr:
			# first: search for complete replacement
			new_expr = self.expressions_dict.get(new_expr, new_expr)
			
			# second: use regex syntax
			for pattern, repl in self.expressions_regex:
				new_expr = re.sub(pattern, repl, new_expr)
			
			# last: start from the longest aliases
			for key in sorted(self.expressions_dict,key=len, reverse=True):
				new_expr = new_expr.replace(key, self.expressions_dict[key])
			
			log.debug("expression expansion: %r => %r", expression, new_expr)
		return new_expr
