
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
template<class TTypes>
class RunLumiEventFilter: public FilterBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "run_lumi_event";
	}

	virtual bool DoesEventPass(event_type const& event, product_type const& product,
	                           setting_type const& settings) const ARTUS_CPP11_OVERRIDE
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
