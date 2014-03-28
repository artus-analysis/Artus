
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/Core/interface/GlobalInclude.h"

/**
   \brief GlobalProducer, for tau decays on generator level.
   
   This producers has the GenParticles as input and will extract the following information from 
   this collection:

   - list of pointers to Higgs particles
   - list of pointers to tau particles as daughters of a certain Higgs
   - list of pointers to tau decay products as daughters of a certain Tau of a certain Higgs 
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

	virtual void ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const
	{
		// Reading Boson PdgId from TauSpinnerSettings.json
		stringvector bosonPdgIdVector = globalSettings.GetBosonPdgId();
		//std::cout << "Boson PdgId:             " << bosonPdgIdVector[0] << std::endl;
		int bosonPdgId;
		std::istringstream(bosonPdgIdVector[0]) >> bosonPdgId;

		for (KGenParticles::iterator part = event.m_genParticles->begin();
			 part != event.m_genParticles->end(); ++part)
		{
			// Filling Higgs, its daughter & granddaughter particles
		        if ((abs(part->pdgId()) == bosonPdgId)&&(part->status()==3))// only Higgs with status 3 are needed
			{ 
				product.m_genHiggs.push_back(&(*part));

				std::vector<KGenParticle*> daughters;
				std::vector<std::vector<KGenParticle*>> gdaughters; 
				for (unsigned int i=0; i<part->daughterIndices.size(); ++i) 
				{
					// Higgs with Status 2 is also considered as Higgs status 3 daughter. We do not need this Particles, because it does not decay in products.
					// This leads to the condition, that we search only for Higgs status 3 daughters with PdgId != 25.
					unsigned int indDaughter = part->daughterIndex(i);
				        if ( (indDaughter < event.m_genParticles->size()) && ( abs((event.m_genParticles->at(indDaughter)).pdgId()) != bosonPdgId) )
				        {      
						// Taus with status 2 are the only daughters of Taus with status 3. We are not interested in status 2 Taus and thats the reason, why we should  
                                		// skip them and consider the formal granddaughters of status 3 Taus as real daughters of status 3 Taus. This means, we must skip one generation,
						// what's done in the following for-loop.
				                daughters.push_back(&(event.m_genParticles->at(indDaughter)));
                                  		std::vector<KGenParticle*> granddaughters;
						unsigned int indDaughterStat2 = (event.m_genParticles->at(indDaughter)).daughterIndex(0);
						for (unsigned int j=0; j<(event.m_genParticles->at(indDaughterStat2)).daughterIndices.size();++j)
						{	
							unsigned int indGranddaughter = (event.m_genParticles->at(indDaughterStat2)).daughterIndex(j);
							if (indGranddaughter < event.m_genParticles->size())
							{
							
								granddaughters.push_back(&(event.m_genParticles->at(indGranddaughter)));
							}
							else
							{
					  			LOG("Granddaughter index larger than size of gen particle vector:" 
					      			<< indGranddaughter << ">" << event.m_genParticles->size() << ".");
							}						
						}
						gdaughters.push_back(granddaughters);					
					}
					else if (!(indDaughter < event.m_genParticles->size()))
					{
					  LOG("Daughter index larger than size of gen particle vector:" 
					      << indDaughter << ">" << event.m_genParticles->size() << ".");
					}
				}
				product.m_genHiggsDaughters.push_back(daughters);
				product.m_genHiggsGranddaughters.push_back(gdaughters);				
			}

		}
	}
};

