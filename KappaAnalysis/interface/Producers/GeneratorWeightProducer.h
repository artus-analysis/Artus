#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

class GeneratorWeightProducer : public KappaProducerBase {
public:

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "GeneratorWeightProducer";
	}

	virtual void Produce(KappaEvent const& event,
			KappaProduct& product,
			KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE
	{
		product.m_weights["generatorWeight"] = event.m_genEventMetadata->weight;
	}

};
