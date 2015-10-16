
#include "Artus/KappaAnalysis/interface/Consumers/EventCountConsumer.h"
#include "Artus/Utility/interface/RootFileHelper.h"

EventCountConsumer::EventCountConsumer() : ConsumerBase<KappaTypes>()
{
	currentLumi = 0;
	m_totalEvents = new TH1I("totalEvents", "totalEvents", 2, -1, 1);
	m_filteredEvents = new TH1I("filteredEvents", "filteredEvents", 2, -1, 1);
}

std::string EventCountConsumer::GetConsumerId() const
{
	return "EventCountConsumer";
}

void EventCountConsumer::ProcessEvent(event_type const& event, product_type const& product, setting_type const& settings, FilterResult & result)
{
	assert(event.m_eventInfo);
	assert(event.m_filterMetadata);
	if(currentLumi != event.m_eventInfo->nLumi)
	{
		currentLumi = event.m_eventInfo->nLumi;
		m_totalEvents->AddBinContent(1, event.m_filterMetadata->nNegEventsTotal);
		m_totalEvents->AddBinContent(2, event.m_filterMetadata->nEventsTotal - event.m_filterMetadata->nNegEventsTotal);
		m_filteredEvents->AddBinContent(1, event.m_filterMetadata->nNegEventsFiltered);
		m_filteredEvents->AddBinContent(2, event.m_filterMetadata->nEventsFiltered - event.m_filterMetadata->nNegEventsTotal);
	}
}

void EventCountConsumer::Finish(setting_type const& settings)
{
	RootFileHelper::SafeCd(settings.GetRootOutFile(), "");
	m_totalEvents->Write(m_totalEvents->GetName());
	m_filteredEvents->Write(m_filteredEvents->GetName());
}

