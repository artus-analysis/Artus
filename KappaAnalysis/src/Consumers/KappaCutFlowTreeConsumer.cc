
#include "Artus/KappaAnalysis/interface/Consumers/KappaCutFlowTreeConsumer.h"


void KappaCutFlowTreeConsumer::Init(setting_type const& settings, metadata_type& metadata)
{
	CutFlowTreeConsumer<KappaTypes>::Init(settings, metadata);

	this->m_runExtractor = [](event_type const& event, product_type const& product, setting_type const& setting) -> int {
		return event.m_eventInfo->nRun;
	};
	this->m_lumiExtractor = [](event_type const& event, product_type const& product, setting_type const& setting) -> int {
		return event.m_eventInfo->nLumi;
	};
	this->m_eventExtractor = [](event_type const& event, product_type const& product, setting_type const& setting) -> uint64_t {
		return event.m_eventInfo->nEvent;
	};
}
