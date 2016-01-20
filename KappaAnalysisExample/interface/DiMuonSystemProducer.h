
#pragma once

#include "Artus/Core/interface/ProducerBase.h"

#include "Artus/KappaAnalysisExample/interface/KappaExampleTypes.h"


class DiMuonSystemProducer: public ProducerBase<KappaExampleTypes> {
public:

	typedef typename KappaExampleTypes::event_type event_type;
	typedef typename KappaExampleTypes::product_type product_type;
	typedef typename KappaExampleTypes::setting_type setting_type;

	virtual std::string GetProducerId() const override;
	
	virtual void Init(setting_type const& settings) override;

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const override;
};

