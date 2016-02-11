
# -*- coding: utf-8 -*-

import re
import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)


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
		return self.expressions_dict.get(expressions.lower(), expression)

	def replace_expressions(self, expression):
		"""Replace any known expressions"""
		# old-style behaviour
		if not self.expressions_dict:
			for key, value in self.expressions_dict.iteritems():
				try:
					expression = new_expr.replace(key, value)
				except AttributeError:
					return expression
			return expression
		# iterative substitution
		if expression is None:
			return expression
		old_expr, new_expr = "", expression
		while new_expr != old_expr:
			old_expr = new_expr
			for key in sorted(self.expressions_dict,key=len, reverse=True):
				new_expr = new_expr.replace(key, self.expressions_dict[key])
			for pattern, repl in self.expressions_regex:
				new_expr = re.sub(pattern, repl, new_expr)
		log.debug("expression expansion: %r => %r", expression, new_expr)
		return new_expr
