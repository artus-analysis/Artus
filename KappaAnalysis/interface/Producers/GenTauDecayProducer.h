
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
	
	virtual void InitGlobal(global_setting_type const& globalSettings)  ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::InitGlobal(globalSettings);
	}
	
	virtual void InitLocal(setting_type const& settings)  ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::InitLocal(settings);
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
			if ((abs(part->pdgId()) == bosonPdgId)&&(part->status()==3))// only Boson with status 3 are considered as root of the tree.
			{
				
				product.m_genBoson.push_back( MotherDaughterBundle(&(*part)) );
				MotherDaughterBundle & lastBosonRef = product.m_genBoson.back();
				lastBosonRef.parent = &lastBosonRef;
				//std::cout << &(lastBosonRef) << std::endl;
				lastBosonRef.setCharge();
				lastBosonRef.setDetectable();
				if (part->daughterIndices.size() == 0) lastBosonRef.finalState = true;
				for (unsigned int i=0; i<part->daughterIndices.size() && part->daughterIndices.size() != 0; ++i) 
				{
					// Higgs with Status 2 is also considered as Higgs status 3 daughter, what leads to the condition, 
					// that we search only for Higgs daughters with PdgId != 25.
					unsigned int indDaughter = part->daughterIndex(i);
					if ( indDaughter < event.m_genParticles->size() )
					{
						// Taus with status 2 are the only daughters of Taus with status 3. We are not interested in status 2 Taus and thats the reason, why we should  
						// skip them and consider the formal granddaughters of status 3 Taus as real daughters of status 3 Taus. This means, we must skip one generation,
						// what's done in the following lines.
						lastBosonRef.Daughters.push_back(MotherDaughterBundle( &(event.m_genParticles->at(indDaughter)) ));
						MotherDaughterBundle & lastBosonDaughterRef = lastBosonRef.Daughters.back();
						//std::cout << &(lastBosonDaughterRef) << std::endl;
						lastBosonDaughterRef.parent = &lastBosonRef;
						lastBosonDaughterRef.setCharge();
						lastBosonDaughterRef.setDetectable();
						if ( (event.m_genParticles->at(indDaughter)).daughterIndices.size() != 0)
						{
							unsigned int indDaughterStat2 = (event.m_genParticles->at(indDaughter)).daughterIndex(0);
							buildDecayTree(lastBosonDaughterRef, indDaughterStat2, event);
						}
						else lastBosonDaughterRef.finalState = true;
					}
					else if (!(indDaughter < event.m_genParticles->size()))
					{
					LOG(ERROR) << "Index larger than size of gen particle vector:" << indDaughter << ">" << event.m_genParticles->size() << ".";
					}
				}
			}
		}
	//std::cout << "Tau1: " << &(product.m_genBoson[0].Daughters[0]) << std::endl;
	//std::cout << "parent ueber Daughter: " << product.m_genBoson[0].Daughters[0].Daughters[0].parent << std::endl;
	//std::cout << std::endl;
	}
	void buildDecayTree(MotherDaughterBundle & lastProductParentRef, unsigned int lastEventParentIndex, event_type const& event) const
	{
		for (unsigned int j=0; j<(event.m_genParticles->at(lastEventParentIndex)).daughterIndices.size() && (event.m_genParticles->at(lastEventParentIndex)).daughterIndices.size() != 0; ++j)
		{
			unsigned int DaughterIndex = (event.m_genParticles->at(lastEventParentIndex)).daughterIndex(j);
			if (DaughterIndex < event.m_genParticles->size())
			{
				lastProductParentRef.Daughters.push_back(MotherDaughterBundle( &(event.m_genParticles->at(DaughterIndex)) ));
				MotherDaughterBundle & lastDaughterRef = lastProductParentRef.Daughters.back();
				lastDaughterRef.parent = &lastProductParentRef;
				//std::cout << "parent selbst im product: " << &lastProductParentRef << std::endl;
				//std::cout << "parent über Daughter im product: " << lastDaughterRef.parent << std::endl;
				lastDaughterRef.setCharge();
				lastDaughterRef.setDetectable();
				if ( (event.m_genParticles->at(DaughterIndex)).daughterIndices.size() == 0) lastDaughterRef.finalState = true;
				else buildDecayTree(lastDaughterRef, DaughterIndex, event);
			}
			else
			{
				LOG (ERROR) << "Index larger than size of gen particle vector:" << DaughterIndex << ">" << event.m_genParticles->size() << ".";
			}
		}
	}
};

