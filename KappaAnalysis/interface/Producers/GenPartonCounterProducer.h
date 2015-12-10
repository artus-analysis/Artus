
#pragma once

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

	void Init(KappaSettings const& settings) override;

	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;

};
