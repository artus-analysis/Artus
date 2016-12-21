
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
