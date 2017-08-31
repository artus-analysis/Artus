
#pragma once

#include "Artus/Core/interface/ProducerBase.h"

#include "Artus/KappaAnalysisExample/interface/KappaExampleTypes.h"


class DiMuonSystemProducer: public ProducerBase<KappaExampleTypes> {
public:

	virtual std::string GetProducerId() const override;
	
	virtual void Init(setting_type const& settings, metadata_type& metadata) override;

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings, metadata_type const& metadata) const override;
};

