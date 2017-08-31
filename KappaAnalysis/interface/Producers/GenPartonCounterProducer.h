
#pragma once

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/**
   \brief GlobalProducer, count partons at generator level.
   
   This producers takes the GenParticles as input and counts the final-state partons which are present in
   an event in addition to a boson.
   
   Required config tag:
     - BosonPdgId
*/

class GenPartonCounterProducer: public ProducerBase<KappaTypes>
{
public:

	std::string GetProducerId() const override;

	void Init(setting_type const& settings, metadata_type& metadata) override;

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override;

};
