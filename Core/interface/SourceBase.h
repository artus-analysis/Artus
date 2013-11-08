/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#ifndef SOURCEBASE_H_
#define SOURCEBASE_H_

#include "GlobalInclude.h"

template < class TEvent, class TProduct, class TSettings >
class SourceBase
{
public:
	virtual bool GetValue(TEvent const& event,
			TProduct const& product,
			TSettings const& settings,
			double & val) const = 0 ;


	virtual void StartOnEvent(TEvent const& event,
			TProduct const& product,
			TSettings const& settings) const {}


	virtual void EndOnEvent(TEvent const& event,
			TProduct const& product,
			TSettings const& settings) const {}


	virtual bool HasDefaultBinCount() const
	{
		return true;
	}

	virtual double HasDefaultBins() const
	{
		return true;
	}


	virtual unsigned int GetDefaultBinCount() const
	{
		LOG_FATAL("implement this for your class")
	}

	virtual double GetDefaultLowBin() const
	{
		LOG_FATAL("implement this for your class")
	}

	virtual double GetDefaultHighBin() const
	{
		LOG_FATAL("implement this for your class")
	}
};


#endif /* SOURCEBASE_H_ */
