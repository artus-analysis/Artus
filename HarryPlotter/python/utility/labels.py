
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)


"""
    This module contains a dictionary with nice (LaTeX) labels.
"""
class LabelsDict(object):
    def __init__(self, latex_version="latex", additional_labels=None):
        self.labels_dict = {}
        
        # examples
        """
        if latex_version == "root":
            self.labels_dict["zpt"] = "Z p_{T} / GeV"
            self.labels_dict["zmass"] = "m_{Z} / GeV"
        else:
            self.labels_dict["zpt"] = "Z $p_\mathrm{T}$ / GeV"
            self.labels_dict["zmass"] = "$m_\mathrm{Z}$ / GeV"
        """
        
        if not additional_labels is None:
            self.labels_dict.update(additional_labels)

    def get_nice_label(self, label):
        if label in self.labels_dict:
            return self.labels_dict.get(label, label)
        elif label.split("+*+")[0] in self.labels_dict:
            return label.replace(label.split("+*+")[0]+"+*+", self.labels_dict[label.split("+*+")[0]])
        else:
            return self.labels_dict.get(label, label)
