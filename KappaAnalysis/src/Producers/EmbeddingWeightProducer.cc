
#include "Artus/KappaAnalysis/interface/Producers/EmbeddingWeightProducer.h"


std::string EmbeddingWeightProducer::GetProducerId() const {
	return "EmbeddingWeightProducer";
}

void EmbeddingWeightProducer::Produce(KappaEvent const& event,
		KappaProduct& product,
		KappaSettings const& settings) const
{
	assert(event.m_eventInfo);

	product.m_weights["embeddingWeight"] = event.m_eventInfo->minVisPtFilterWeight;
}

