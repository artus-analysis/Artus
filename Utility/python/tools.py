
# -*- coding: utf-8 -*-


# flatten 2D list
# return [1, 2, 3, 4, ...] for input [[1, 2], [3, 4, ...], ...]
def flattenList(listOfLists):
	return [item for subList in listOfLists for item in subList]
