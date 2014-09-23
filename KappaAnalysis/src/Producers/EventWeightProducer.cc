
#include "Artus/KappaAnalysis/interface/Producers/EventWeightProducer.h"

std::string EventWeightProducer::GetProducerId() const {
	return "EventWeightProducer";
}

void EventWeightProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
{
	double eventWeight = 1.0;

	// loop over all previously calculated weights and multiply them
	for(std::map<std::string, double>::const_iterator weight = product.m_weights.begin();
		weight != product.m_weights.end(); ++weight)
	{
		eventWeight *= weight->second;
	}

	product.m_weights[settings.GetEventWeight()] = eventWeight;
}


