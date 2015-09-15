#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

class LuminosityWeightProducer : public KappaProducerBase {
public:

	virtual std::string GetProducerId() const override;

	virtual void Produce(KappaEvent const& event,
			KappaProduct& product,
			KappaSettings const& settings) const override;

};
