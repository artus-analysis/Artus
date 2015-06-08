#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

class LuminosityWeightProducer : public KappaProducerBase {
public:

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;

	virtual void Produce(KappaEvent const& event,
			KappaProduct& product,
			KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE;

};
