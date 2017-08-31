
#include "Artus/KappaAnalysis/interface/Producers/NumberGeneratedEventsWeightProducer.h"


std::string NumberGeneratedEventsWeightProducer::GetProducerId() const {
	return "NumberGeneratedEventsWeightProducer";
}

void NumberGeneratedEventsWeightProducer::Produce(event_type const& event, product_type & product,
                                                  setting_type const& settings, metadata_type const& metadata) const
{
	product.m_weights["numberGeneratedEventsWeight"] = (1.0 / settings.GetNumberGeneratedEvents());
}

