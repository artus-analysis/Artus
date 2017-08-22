
#include "Artus/KappaAnalysis/interface/Consumers/PrintEventsConsumer.h"

PrintEventsConsumer::PrintEventsConsumer() : ConsumerBase<KappaTypes>()
{
}

std::string PrintEventsConsumer::GetConsumerId() const
{
	return "PrintEventsConsumer";
}

void PrintEventsConsumer::ProcessEvent(event_type const& event, product_type const& product,
                                       setting_type const& settings, FilterResult& result)
{
	// LOG(INFO) << "Processed event: run = " << event.m_eventInfo->nRun << "/" << event.m_lumiInfo->nRun << "/" << event.m_runInfo->nRun << ", lumi = " << event.m_eventInfo->nLumi << "/" << event.m_lumiInfo->nLumi << ", event = " << event.m_eventInfo->nEvent << ", pipeline = " << settings.GetName();
	LOG(INFO) << "Processed event: run = " << event.m_eventInfo->nRun << ", lumi = " << event.m_eventInfo->nLumi << ", event = " << event.m_eventInfo->nEvent << ", pipeline = " << settings.GetName();
	LOG(INFO) << "====================================================================================================\n";
}

void PrintEventsConsumer::Finish(setting_type const& settings)
{
}

