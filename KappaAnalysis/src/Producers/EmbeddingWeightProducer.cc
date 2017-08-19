
#include "Artus/KappaAnalysis/interface/Producers/EmbeddingWeightProducer.h"


std::string EmbeddingWeightProducer::GetProducerId() const {
	return "EmbeddingWeightProducer";
}

void EmbeddingWeightProducer::Produce(event_type const& event, product_type& product,
                                      setting_type const& settings, metadata_type const& metadata) const
{
	assert(event.m_eventInfo);

	product.m_weights["embeddingWeight"] = event.m_eventInfo->minVisPtFilterWeight;
}

