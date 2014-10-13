#include "Artus/KappaAnalysis/interface/Filters/RunLumiEventFilter.h"

std::string RunLumiEventFilter::GetFilterId() const {
	return "RunLumiEventFilter";
}

bool RunLumiEventFilter::DoesEventPass(KappaEvent const& event, KappaProduct const& product,
                           KappaSettings const& settings) const 
{
	assert(event.m_eventMetadata);
	
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

bool RunLumiEventFilter::MatchWhiteBlackLists(int item, std::vector<int> const& whitelist, std::vector<int> const& blacklist) const
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
