#include "Artus/KappaAnalysis/interface/Filters/RunLumiEventFilter.h"

std::string RunLumiEventFilter::GetFilterId() const {
	return "RunLumiEventFilter";
}

bool RunLumiEventFilter::DoesEventPass(event_type const& event, product_type const& product,
                                       setting_type const& settings, metadata_type const& metadata) const
{
	assert(event.m_eventInfo);

	bool match = false;

	if (settings.GetMatchRunLumiEventTuples())
	{
		match = true;
		for (size_t index = 0; index < std::min(std::min(settings.GetRunBlacklist().size(),
		                                                 settings.GetLumiBlacklist().size()),
		                                        settings.GetEventBlacklist().size()); ++index)
		{
			if ((event.m_eventInfo->nRun == settings.GetRunBlacklist()[index]) &&
			    (event.m_eventInfo->nLumi == settings.GetLumiBlacklist()[index]) &&
			    (event.m_eventInfo->nEvent == settings.GetEventBlacklist()[index]))
			{
				match = false;
				break;
			}
		}
		if (match)
		{
			match = false;
			for (size_t index = 0; index < std::min(std::min(settings.GetRunWhitelist().size(),
			                                                 settings.GetLumiWhitelist().size()),
			                                        settings.GetEventWhitelist().size()); ++index)
			{
				if ((event.m_eventInfo->nRun == settings.GetRunWhitelist()[index]) &&
					(event.m_eventInfo->nLumi == settings.GetLumiWhitelist()[index]) &&
					(event.m_eventInfo->nEvent == settings.GetEventWhitelist()[index]))
				{
					match = true;
					break;
				}
			}
		}
	}
	else
	{
		match = (MatchWhiteBlackLists(event.m_eventInfo->nRun, settings.GetRunWhitelist(), settings.GetRunBlacklist()) &&
		         MatchWhiteBlackLists(event.m_eventInfo->nLumi, settings.GetLumiWhitelist(), settings.GetLumiBlacklist()) &&
		         MatchWhiteBlackLists(event.m_eventInfo->nEvent, settings.GetEventWhitelist(), settings.GetEventBlacklist()));
	}
	if (match)
	{
		LOG(DEBUG) << "Process: " <<
		              "run = " << event.m_eventInfo->nRun << ", " <<
		              "lumi = " << event.m_eventInfo->nLumi << ", " <<
		              "event = " << event.m_eventInfo->nEvent;
	}
	return match;
}

bool RunLumiEventFilter::MatchWhiteBlackLists(uint64_t item, std::vector<uint64_t> const& whitelist, std::vector<uint64_t> const& blacklist) const
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
