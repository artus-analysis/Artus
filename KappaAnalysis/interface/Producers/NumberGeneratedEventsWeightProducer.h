#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

/**
   \brief NumberGeneratedEventsWeightProducer
   Config tags:
   - NumberGeneratedEvents, e.g. "1234"

*/

class NumberGeneratedEventsWeightProducer : public KappaProducerBase {
public:

	virtual std::string GetProducerId() const override;

	virtual void Produce(KappaEvent const& event,
	                     KappaProduct & product,
	                     KappaSettings const& settings) const override;

};
