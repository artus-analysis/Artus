
#include "Artus/KappaAnalysis/interface/Consumers/PrintEventsConsumer.h"

PrintEventsConsumer::PrintEventsConsumer() : ConsumerBase<KappaTypes>()
{
}

std::string PrintEventsConsumer::GetConsumerId() const
{
	return "PrintEventsConsumer";
}

void PrintEventsConsumer::ProcessFilteredEvent(event_type const& event, product_type const& product,
                                            setting_type const& settings)
{
	LOG(INFO) << "Processed event: run = " << event.m_eventInfo->nRun << ", lumi = " << event.m_eventInfo->nLumi << ", event = " << event.m_eventInfo->nEvent;
	LOG(INFO) << "====================================================================================================\n";
}

void PrintEventsConsumer::Finish(setting_type const& settings)
{
}

