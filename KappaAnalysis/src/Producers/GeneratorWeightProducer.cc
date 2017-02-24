
#include "Artus/KappaAnalysis/interface/Producers/GeneratorWeightProducer.h"


void GeneratorWeightProducer::Produce(KappaEvent const& event,
		KappaProduct& product,
		KappaSettings const& settings) const
{
	assert(event.m_genEventInfo);

	// if available, use a precomputed generator weight which takes into account negative NLO weights
	if (settings.GetGeneratorWeight() != DefaultValues::UndefinedDouble)
	{
		// convert the weight into +/- 1.0 (the absolute value is not meaningful)
		double weight = event.m_genEventInfo->weight / std::abs(event.m_genEventInfo->weight);

		// store this weight, normalizing it to the sum of weights (positive and negative) 
		// computed before any selection is applied
		product.m_weights["generatorWeight"] = (weight / settings.GetGeneratorWeight());
	}
	// otherwise retrieve it, on an event-basis, from the input file
	else
	{
		product.m_weights["generatorWeight"] = event.m_genEventInfo->weight;
	}
}

