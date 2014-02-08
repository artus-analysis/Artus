/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/noncopyable.hpp>

template<class TTypes>
class EventProviderBase: public boost::noncopyable {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::global_setting_type global_setting_type;

	virtual event_type const& GetCurrentEvent() const = 0;
	virtual bool GetEntry(long long lEventNumber) = 0;
	virtual long long GetEntries() const = 0;
};
