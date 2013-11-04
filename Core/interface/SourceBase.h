/*
 * SourceBase.h
 *
 *  Created on: Sep 21, 2011
 *      Author: poseidon
 */

#ifndef SOURCEBASE_H_
#define SOURCEBASE_H_

#include "GlobalInclude.h"

template < class TEvent, class TMetaData, class TSettings >
class SourceBase
{
public:
	virtual bool GetValue(TEvent const& event,
			TMetaData const& metaData,
			TSettings const& settings,
			double & val) const = 0 ;


	virtual void StartOnEvent(TEvent const& event,
			TMetaData const& metaData,
			TSettings const& settings) const {}


	virtual void EndOnEvent(TEvent const& event,
			TMetaData const& metaData,
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
		CALIB_LOG_FATAL("implement this for your class")
	}

	virtual double GetDefaultLowBin() const
	{
		CALIB_LOG_FATAL("implement this for your class")
	}

	virtual double GetDefaultHighBin() const
	{
		CALIB_LOG_FATAL("implement this for your class")
	}
};


#endif /* SOURCEBASE_H_ */
