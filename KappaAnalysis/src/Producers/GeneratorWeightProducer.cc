
#include "Artus/KappaAnalysis/interface/Producers/GeneratorWeightProducer.h"


std::string GeneratorWeightProducer::GetProducerId() const {
	return "GeneratorWeightProducer";
}

void GeneratorWeightProducer::Produce(KappaEvent const& event,
		KappaProduct& product,
		KappaSettings const& settings) const
{
	assert(event.m_genEventInfo);

	// if available, use a precomputed generator weight which takes into account negative NLO weights
	if (settings.GetGeneratorWeight() != DefaultValues::UndefinedInt)
	{
		product.m_weights["generatorWeight"] = (1.0 / settings.GetGeneratorWeight());
	}
	// otherwise retrieve it, on an event-basis, from the input file
	else
	{
		product.m_weights["generatorWeight"] = event.m_genEventInfo->weight;
	}
}

