
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
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "run_lumi_event";
	}

	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE
	{
		FilterBase<TTypes>::InitGlobal(globalSettings);
		
		runWhitelist = globalSettings.GetRunWhitelist();
		runBlacklist = globalSettings.GetRunBlacklist();
		lumiWhitelist = globalSettings.GetLumiWhitelist();
		lumiBlacklist = globalSettings.GetLumiBlacklist();
		eventWhitelist = globalSettings.GetEventWhitelist();
		eventBlacklist = globalSettings.GetEventBlacklist();
	}

	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE
	{
		FilterBase<TTypes>::InitLocal(settings);
		
		runWhitelist = settings.GetRunWhitelist();
		runBlacklist = settings.GetRunBlacklist();
		lumiWhitelist = settings.GetLumiWhitelist();
		lumiBlacklist = settings.GetLumiBlacklist();
		eventWhitelist = settings.GetEventWhitelist();
		eventBlacklist = settings.GetEventBlacklist();
	}

	virtual bool DoesEventPassGlobal(event_type const& event, product_type const& product,
	                                 global_setting_type const& globalSettings) const ARTUS_CPP11_OVERRIDE
	{
		return DoesEventPass(event, product);
	}

	virtual bool DoesEventPassLocal(event_type const& event, product_type const& product,
	                                setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		return DoesEventPass(event, product);
	}


protected:

	virtual bool DoesEventPass(event_type const& event, product_type const& product) const
	{
		return (MatchWhiteBlackLists(event.m_eventMetadata->nRun, runWhitelist, runBlacklist) &&
		        MatchWhiteBlackLists(event.m_eventMetadata->nLumi, lumiWhitelist, lumiBlacklist) &&
		        MatchWhiteBlackLists(event.m_eventMetadata->nEvent, eventWhitelist, eventBlacklist));
	}


private:
	std::vector<int> runWhitelist;
	std::vector<int> runBlacklist;
	std::vector<int> lumiWhitelist;
	std::vector<int> lumiBlacklist;
	std::vector<int> eventWhitelist;
	std::vector<int> eventBlacklist;
	
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
