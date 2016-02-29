#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

/**
   \brief SampleStitchingWeightProducer
   Config tags:
   - Fill me with something meaningful

*/

class SampleStitchingWeightProducer : public KappaProducerBase {
public:

	std::string GetProducerId() const override;

	void Produce( KappaEvent const& event,
			KappaProduct & product,
			KappaSettings const& settings) const override;

};
