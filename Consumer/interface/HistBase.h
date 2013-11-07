/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "PlotBase.h"

/*

Base class for all wrappers which hold histograms

*/

template<class THistType>
class HistBase: public PlotBase<THistType> {
public:
	HistBase(std::string sName, std::string sFolder) :
			PlotBase<THistType>(sName, sFolder) {
	}
};
