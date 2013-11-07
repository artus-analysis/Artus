/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

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
