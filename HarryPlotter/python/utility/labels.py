#  -*- coding: utf-8 -*-

"""
	This module contains a dictionary with nice (LaTeX) labels.
"""


class LabelsDict(object):
	def __init__(self, additional_labels=None):
		self.labels_dict = {
			'zpt': 'Z $p_\mathrm{T}$ / GeV',
			'zmass': '$m_\mathrm{Z}$ / GeV',
		}
		if additional_labels != None:
			self.labels_dict.update(additional_labels)

	def get_nice_label(self, label):
		"""This function is needed to encapsulate access to labels_dict."""
		return self.labels_dict.get(*(label,)*2)
