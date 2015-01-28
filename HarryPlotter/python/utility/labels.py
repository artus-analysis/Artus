#  -*- coding: utf-8 -*-

"""
	This module contains a dictionary with nice (LaTeX) labels.
"""

labels_dict = {
    'zpt': 'Z $p_\mathrm{T}$ / GeV'
}

def get_nice_label(label):
	"""This function is needed to encapsulate access to labels_dict."""
	return labels_dict.get(*(label,)*2)
