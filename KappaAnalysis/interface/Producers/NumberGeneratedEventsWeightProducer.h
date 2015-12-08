#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

/**
   \brief NumberGeneratedEventsWeightProducer
   Config tags:
   - NumberGeneratedEvents, e.g. "1234"

*/

class NumberGeneratedEventsWeightProducer : public KappaProducerBase {
public:

	std::string GetProducerId() const override;

	void Produce(KappaEvent const& event,
	                     KappaProduct & product,
	                     KappaSettings const& settings) const override;

};
