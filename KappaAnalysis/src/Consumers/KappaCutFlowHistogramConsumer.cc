
#include "Artus/KappaAnalysis/interface/Consumers/KappaCutFlowHistogramConsumer.h"


void KappaCutFlowHistogramConsumer::Init(KappaSettings const& settings)
{
	CutFlowHistogramConsumer<KappaTypes>::Init(settings);

	this->weightExtractor = [](event_type const& event, product_type const& product, setting_type const& setting) -> float {
		return SafeMap::GetWithDefault(product.m_weights, std::string(setting.GetEventWeight()), 1.0);
	};

	this->m_addWeightedCutFlow = true;
}
