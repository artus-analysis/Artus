#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

class LuminosityWeightProducer : public KappaProducerBase {

public:
	void Produce(KappaEvent const& event,
			KappaProduct& product,
			KappaSettings const& settings) const override;
};
