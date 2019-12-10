
#pragma once

#include <boost/noncopyable.hpp>

template<class TTypes>
class EventProviderBase: public boost::noncopyable {
public:

	virtual ~EventProviderBase()
	{
	}

	typedef typename TTypes::event_type event_type;

	virtual event_type const& GetCurrentEvent() const = 0;

	// returns false for non-existing entries, e.g. when lEventNumber >= GetEntries()
	// this is important as termination condition of the event loop in the PipelineRunner
	virtual bool GetEntry(long long lEventNumber) = 0;

	virtual long long GetEntries() const = 0;
	virtual bool NewLumisection() const { return false; }
	virtual bool NewRun() const { return false; }

};
