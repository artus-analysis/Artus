
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "KappaTools/RootTools/RunLumiReader.h"

#include "Artus/Core/interface/FilterBase.h"


/** Filter events by white/black lists for run/lumi/event numbers
 *  Config tags:
 *   - RunWhitelist
 *   - RunBlacklist
 *   - LumiWhitelist
 *   - LumiBlacklist
 *   - EventWhitelist
 *   - EventBlacklist
 */
class RunLumiEventFilter: public FilterBase<KappaTypes>
{

public:


	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "RunLumiEventFilter";
	}

	virtual bool DoesEventPass(KappaEvent const& event, KappaProduct const& product,
	                           KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE
	{
		bool match = (MatchWhiteBlackLists(event.m_eventMetadata->nRun, settings.GetRunWhitelist(), settings.GetRunBlacklist()) &&
		              MatchWhiteBlackLists(event.m_eventMetadata->nLumi, settings.GetLumiWhitelist(), settings.GetLumiBlacklist()) &&
		              MatchWhiteBlackLists(event.m_eventMetadata->nEvent, settings.GetEventWhitelist(), settings.GetEventBlacklist()));
		if (match)
		{
			LOG(DEBUG) << "Process: " <<
			              "run = " << event.m_eventMetadata->nRun << ", " <<
			              "lumi = " << event.m_eventMetadata->nLumi << ", " <<
			              "event = " << event.m_eventMetadata->nEvent;
		}
		return match;
	}


private:
	
	bool MatchWhiteBlackLists(int item, std::vector<int> const& whitelist, std::vector<int> const& blacklist) const
	{
		if ((whitelist.size() > 0) && (std::find(whitelist.begin(), whitelist.end(), item) == whitelist.end()))
		{
			return false;
		}
		if ((blacklist.size() > 0) && (std::find(blacklist.begin(), blacklist.end(), item) != blacklist.end()))
		{
			return false;
		}
		return true;
	}
};
