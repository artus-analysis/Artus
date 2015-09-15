
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

	virtual std::string GetProducerId() const override;

	virtual void Init(KappaSettings const& settings) override;

	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;

};
