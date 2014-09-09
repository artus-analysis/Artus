#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

/**
   \brief CrossSectionWeightProducer
   Config tags:
   - CrossSection, e.g. "1234"
   If config tag available, provided value is used as weight.
   If not available, external or internal cross section of the
   LumiSection is used.

*/

class CrossSectionWeightProducer : public KappaProducerBase {
public:

	virtual std::string GetProducerId() const
		ARTUS_CPP11_OVERRIDE;

	virtual void Produce( KappaEvent const& event,
			KappaProduct & product,
			KappaSettings const& settings) const
		ARTUS_CPP11_OVERRIDE;

};
