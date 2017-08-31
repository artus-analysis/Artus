#pragma once

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
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

		void Init(setting_type const& settings, metadata_type& metadata) override;
		
		std::string GetProducerId() const override
		{
			return "NumberOfParticlesProducer";
		};
		
		void Produce(event_type const& event, product_type& product,
		             setting_type const& settings, metadata_type const& metadata) const override;
};
