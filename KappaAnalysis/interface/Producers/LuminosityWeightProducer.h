#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

class LuminosityWeightProducer : public KappaProducerBase {
public:

	std::string GetProducerId() const override;

	void Produce(KappaEvent const& event,
			KappaProduct& product,
			KappaSettings const& settings) const override;

};
