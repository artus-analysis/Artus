#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"

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

	std::string GetProducerId() const override;

	void Produce(event_type const& event, product_type & product,
	             setting_type const& settings, metadata_type const& metadata) const override;

};
