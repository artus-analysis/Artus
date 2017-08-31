
#include "Artus/KappaAnalysis/interface/Consumers/KappaCutFlowHistogramConsumer.h"


void KappaCutFlowHistogramConsumer::Init(setting_type const& settings, metadata_type& metadata)
{
	CutFlowHistogramConsumer<KappaTypes>::Init(settings, metadata);

	this->weightExtractor = [](event_type const& event, product_type const& product, setting_type const& setting) -> double {
		return SafeMap::GetWithDefault(product.m_weights, setting.GetEventWeight(), 1.0);
	};

	this->m_addWeightedCutFlow = true;
}
