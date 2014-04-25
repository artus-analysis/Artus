
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"

#include "Artus/KappaAnalysis/interface/KappaProduct.h"

/**
   \brief GlobalProducer, for tau decays on generator level.
   
   This producers has the GenParticles as input and will extract the following information from 
   this collection :

   - tree with three generations of decay products : Boson, Bosondaughters, Bosongranddaughters  

   If need arises to store other decay trees, this code can be made more general and
   configurable.
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
		int bosonPdgId;
		std::istringstream(bosonPdgIdVector[0]) >> bosonPdgId;

		for (KGenParticles::iterator part = event.m_genParticles->begin();
			 part != event.m_genParticles->end(); ++part)
		{
			// Filling Higgs, its daughter & granddaughter particles
			if ((abs(part->pdgId()) == bosonPdgId)&&(part->status()==3))// only Boson with status 3 are considered
			{
				
				product.m_genBoson.push_back( KappaProduct::MotherDaughterBundle(&(*part)) );
				KappaProduct::MotherDaughterBundle & lastBosonRef = product.m_genBoson.back();

				for (unsigned int i=0; i<part->daughterIndices.size(); ++i) 
				{
					// Higgs with Status 2 is also considered as Higgs status 3 daughter, what leads to the condition, 
					// that we search only for Higgs daughters with PdgId != 25.
					unsigned int indDaughter = part->daughterIndex(i);
					if ( (indDaughter < event.m_genParticles->size()) && ( abs((event.m_genParticles->at(indDaughter)).pdgId()) != bosonPdgId) )
					{
						// Taus with status 2 are the only daughters of Taus with status 3. We are not interested in status 2 Taus and thats the reason, why we should  
						// skip them and consider the formal granddaughters of status 3 Taus as real daughters of status 3 Taus. This means, we must skip one generation,
						// what's done in the following for-loop.

						lastBosonRef.Daughters.push_back(KappaProduct::MotherDaughterBundle( &(event.m_genParticles->at(indDaughter)) ));
						KappaProduct::MotherDaughterBundle & lastDaughterRef = lastBosonRef.Daughters.back();

						unsigned int indDaughterStat2 = (event.m_genParticles->at(indDaughter)).daughterIndex(0);
						for (unsigned int j=0; j<(event.m_genParticles->at(indDaughterStat2)).daughterIndices.size();++j)
						{	
							unsigned int indGranddaughter = (event.m_genParticles->at(indDaughterStat2)).daughterIndex(j);
							if (indGranddaughter < event.m_genParticles->size())
							{
							
								lastDaughterRef.Daughters.push_back(KappaProduct::MotherDaughterBundle( &(event.m_genParticles->at(indGranddaughter)) ));
							}
							else
							{
								LOG (ERROR) << "Granddaughter index larger than size of gen particle vector:" << indGranddaughter << ">" << event.m_genParticles->size() << ".";
							}
						}
					}
					else if (!(indDaughter < event.m_genParticles->size()))
					{
					LOG(ERROR) << "Daughter index larger than size of gen particle vector:" << indDaughter << ">" << event.m_genParticles->size() << ".";
					}
				}
			}
		}
	}
};

