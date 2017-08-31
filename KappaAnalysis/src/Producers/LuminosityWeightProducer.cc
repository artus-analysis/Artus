
#include "Artus/KappaAnalysis/interface/Producers/LuminosityWeightProducer.h"


std::string LuminosityWeightProducer::GetProducerId() const {
	return "LuminosityWeightProducer";
}

void LuminosityWeightProducer::Produce(event_type const& event, product_type& product,
                                       setting_type const& settings, metadata_type const& metadata) const
{
	product.m_weights["luminosityWeight"] = (1.0 / static_cast<double>(settings.GetIntLuminosity()));
}

