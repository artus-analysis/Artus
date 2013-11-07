/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/noncopyable.hpp>

template<class TEvent>
class EventProviderBase: public boost::noncopyable {
public:

	virtual TEvent const& GetCurrentEvent() const = 0;
	virtual bool GotoEvent(long long lEventNumber) = 0;
	virtual long long GetOverallEventCount() const = 0;
};
