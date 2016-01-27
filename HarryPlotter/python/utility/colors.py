
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)


"""
    This module contains a dictionary with predefined colors.
    
    The colors should be named like #RRGGBB (with capital letters), to be used in all plotting backends.
"""
class ColorsDict(object):
    def __init__(self, color_scheme="default", additional_colors=None):
        self.colors_dict = {}
        
        # examples
        """
        self.colors_dict["data"] = "#000000"
        if color_scheme=="default":
            self.colors_dict["graph1"] = "#FF0000"
        """
        
        if not additional_colors is None:
            self.colors_dict.update(additional_colors)

    def get_predefined_color(self, color):
        if color in self.colors_dict:
            return self.colors_dict.get(color, color)
        elif color.split("+*+")[0] in self.colors_dict:
            return self.colors_dict.get(color.split("+*+")[0], color)
        else:
            return self.colors_dict.get(color, color)
