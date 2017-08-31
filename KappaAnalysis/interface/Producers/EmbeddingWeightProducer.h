#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"

class EmbeddingWeightProducer : public KappaProducerBase {
public:

	std::string GetProducerId() const override;

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override;

};
