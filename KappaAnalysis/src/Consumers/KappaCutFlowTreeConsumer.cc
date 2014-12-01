
#include "Artus/KappaAnalysis/interface/Consumers/KappaCutFlowTreeConsumer.h"


void KappaCutFlowTreeConsumer::Init(KappaSettings const& settings)
{
	CutFlowTreeConsumer<KappaTypes>::Init(settings);

	this->m_runExtractor = [](event_type const& event, product_type const& product, setting_type const& setting) -> float {
		return event.m_eventInfo->nRun;
	};
	this->m_lumiExtractor = [](event_type const& event, product_type const& product, setting_type const& setting) -> float {
		return event.m_eventInfo->nLumi;
	};
	this->m_eventExtractor = [](event_type const& event, product_type const& product, setting_type const& setting) -> float {
		return event.m_eventInfo->nEvent;
	};
}
