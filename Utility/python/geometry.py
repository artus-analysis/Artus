
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import itertools
import math
import sys


def distance_2d(point_a, point_b):
	return math.hypot(point_a[0]-point_b[0], point_a[1]-point_b[1])

def distance_2d_between_lists(lists_of_points_2d):
	return sum([distance_2d(first[-1], second[0]) for first, second in zip(lists_of_points_2d[:-1], lists_of_points_2d[1:])])

def order_lists_for_smallest_distances(lists_of_points_2d, allow_reversed):
	lists = list(lists_of_points_2d)
	min_distance = sys.float_info.max
	min_permutation = None
	for index in xrange(0 if allow_reversed else len(lists), len(lists)+1):
		tmp_lists = None
		if index == len(lists):
			tmp_lists = lists
		else:
			tmp_lists = lists[:index]+[lists[index][::-1]]+lists[index+1:]
		for permutation in itertools.permutations(tmp_lists):
			distance = distance_2d_between_lists(permutation)
			if distance < min_distance:
				min_distance = distance
				min_permutation = permutation
	return min_permutation

