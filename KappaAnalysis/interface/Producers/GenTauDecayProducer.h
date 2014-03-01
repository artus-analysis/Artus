
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"


/**
   \brief GlobalProducer, for tau decays on generator level.
   
   This producers has the GenParticles as input and will extract the following information from 
   this collection:

   - pointer to tau particle
   - list of pointers to tau decay products
*/

template<class TTypes>
class GenTauDecayProducer: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "gen_taudecay";
	}

	virtual bool ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const
	{
		for (KGenParticles::iterator part = event.m_genParticles->begin();
			 part != event.m_genParticles->end(); ++part)
		{
		        if (abs(part->pdgId()) == 15)
			{
			        product.m_genTauDecay.push_back(&(*part));
				for (unsigned int d=0; d<part->daughterIndices.size(); ++d)
				{
				        if (part->daughterIndex(d) < event.m_genParticles->size())
				        {
				                product.m_genTauDecay.push_back(&(event.m_genParticles->at(part->daughterIndex(d))));
					}
					else
					{
					  LOG("Daughter index larger than size of gen particle vector:" 
					      << part->daughterIndex(d) << ">" << event.m_genParticles->size() << ".");
					}
				}
			}
		}

		return true;
	}

	// empty to serve as a pure global producer
	virtual void ProduceLocal(event_type const& event,
	                          product_type& product,
	                          setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
	}
};

