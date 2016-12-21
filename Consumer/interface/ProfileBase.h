
#pragma once

#include "PlotBase.h"

template < class THistType >
class ProfileBase: public PlotBase<THistType>
{
public:
	ProfileBase( std::string sName, std::string sFolder ) :
		PlotBase< THistType>(  sName, sFolder)
		{
		}
};
