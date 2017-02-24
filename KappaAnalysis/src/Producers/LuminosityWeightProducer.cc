
#include "Artus/KappaAnalysis/interface/Producers/LuminosityWeightProducer.h"


void LuminosityWeightProducer::Produce(KappaEvent const& event,
		KappaProduct& product,
		KappaSettings const& settings) const
{
	product.m_weights["luminosityWeight"] = (1.0 / static_cast<double>(settings.GetIntLuminosity()));
}

