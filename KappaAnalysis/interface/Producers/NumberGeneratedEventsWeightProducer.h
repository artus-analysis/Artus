#pragma once

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

/**
   \brief NumberGeneratedEventsWeightProducer
   Config tags:
   - NumberGeneratedEvents, e.g. "1234"

*/

class NumberGeneratedEventsWeightProducer : public KappaProducerBase {
public:

	std::string GetProducerId() const override;

	void Produce(event_type const& event, product_type & product,
	             setting_type const& settings, metadata_type const& metadata) const override;

};
