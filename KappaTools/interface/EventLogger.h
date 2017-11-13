/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 *   Manuel Zeise <zeise@cern.ch>
 */

#ifndef KAPPA_EVENTLOGGER_H
#define KAPPA_EVENTLOGGER_H

#include <iostream>
#include <Kappa/DataFormats/interface/KInfo.h>

class EventID
{
public:
	EventID(const KEventInfo *eventinfo) :
		nEvent(eventinfo->nEvent), nLumi(eventinfo->nLumi),
		nRun(eventinfo->nRun), nBX(eventinfo->nBX) {};

	bool operator<(const EventID &other) const
	{
		if (nRun < other.nRun)
			return true;
		else if (nLumi < other.nLumi)
			return true;
		else if (nEvent < other.nEvent)
			return true;
		else if (nBX < other.nBX)
			return true;
		else
			return false;
	}

	event_id nEvent;
	lumi_id nLumi;
	run_id nRun;
	bx_id nBX;
};

std::ostream &operator<<(std::ostream &output, const EventID &id);

template<typename TContainer>
class EventLogger
{
public:
	EventLogger() {}
	void clear() { store.clear(); }
	bool has(const KEventInfo *eventinfo) { return store.find(typename TContainer::value_type(eventinfo)) != store.end(); }
	void put(const KEventInfo *eventinfo) { store.insert(store.begin(), typename TContainer::value_type(eventinfo)); }
	void print()
	{
		for (typename TContainer::const_iterator it = store.begin(); it != store.end(); ++it)
			std::cout << *it << std::endl;
	}
private:
	TContainer store;
};

#endif
