
#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/**
   \brief GlobalProducer, count partons at generator level.
   
   This producers takes the GenParticles as input and counts the final-state partons which are present in
   an event in addition to a Z or W boson
   
   At the moment the choice to start counting after finding a Z or W boson (pdgId = 23 || 24) is hard-coded,
   but, in case of future needs, these pdgIds can be easily taken from a configurable setting.
*/

class GenPartonCounterProducer: public ProducerBase<KappaTypes>
{
public:

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;

	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE;

	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE;

};
