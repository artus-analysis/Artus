#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"

/**
   \brief Producer, that counts the number of particles with various requiremenets chosen without applying these
   requirements for analysis.
*/

class NumberOfParticlesProducer : public KappaProducerBase
{
	public:
		void Init(KappaSettings const& settings) override;
		void Produce(KappaEvent const& event, KappaProduct& product, KappaSettings const& settings) const override;
};
