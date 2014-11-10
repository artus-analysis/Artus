
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/Utility/interface/DefaultValues.h"


/** Abstract Producer class for trigger matching valid objects
 *
 *	Needs to run after the valid object producers.
 */
template<class TValidObject>
class GenMatchingProducerBase: public KappaProducerBase
{

public:
	
	enum class TauDecayMode : int
	{
			NONE = -1,
		E   = 0,
		M   = 1,
		T   = 2
	};

	typedef typename KappaTypes::event_type event_type;
	typedef typename KappaTypes::product_type product_type;
	typedef typename KappaTypes::setting_type setting_type;
	
	GenMatchingProducerBase(std::map<TValidObject*, KGenParticle*> product_type::*genMatchedObjects, //changed to KGenParticle from const KDataLV
	                        std::vector<TValidObject*> product_type::*validObjects,
	                        std::vector<TValidObject*> product_type::*invalidObjects,
	                        TauDecayMode tauDecayMode,
	                        float (setting_type::*GetDeltaRGenMatchingObjects)(void) const,
	                        bool (setting_type::*GetInvalidateNonGenMatchingObjects)(void) const,
	                        std::string (setting_type::*GetMatchingAlgorithmusObjects)(void) const) :
		m_genMatchedObjects(genMatchedObjects),
		m_validObjects(validObjects),
		m_invalidObjects(invalidObjects),
		tauDecayMode(tauDecayMode),
		GetDeltaRGenMatchingObjects(GetDeltaRGenMatchingObjects),
		GetInvalidateNonGenMatchingObjects(GetInvalidateNonGenMatchingObjects),
		GetMatchingAlgorithmusObjects(GetMatchingAlgorithmusObjects)
	{
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE 
	{
		KappaProducerBase::Init(settings);
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("ratioGenMatched", [](event_type const & event, product_type const & product)
		{
			return product.m_ratioGenMatched;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genMatchDeltaR", [](event_type const & event, product_type const & product)
		{
			return product.m_genMatchDeltaR;
		});
	}

	virtual void Produce(event_type const& event, product_type& product,
						 setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		float ratioGenMatched = 0;
		// matching jets to partonen and getting the jet flavour
		if(tauDecayMode == TauDecayMode::NONE)
		{
			assert(event.m_genParticles);
			
			if ((settings.*GetDeltaRGenMatchingObjects)() > 0.0)
			{
				// loop over all valid objects (jets) to check
				for (typename std::vector<TValidObject*>::iterator validObject = (product.*m_validObjects).begin();
					 validObject != (product.*m_validObjects).end();)
				{
					bool objectMatched = false;
					float deltaR = 0;
					KGenParticles matching_algo_partons;
					KGenParticles matching_phys_partons;
					matching_algo_partons.clear();
					matching_phys_partons.clear();
					KGenParticle * hardest_phys_parton = NULL;
					KGenParticle * hardest_parton = NULL;
					KGenParticle * hardest_b_quark = NULL;
					KGenParticle * hardest_c_quark = NULL;
					
					// loop over all genParticles 
					for (auto genParticle=event.m_genParticles->begin(); genParticle!=event.m_genParticles->end(); ++genParticle)
					{
						// only use genParticles with id 21, 1, -1, 2, -2, 3, -3, 4, -4, 5, -5
						if ( (abs(genParticle->pdgId())==1 || abs(genParticle->pdgId())==2 || abs(genParticle->pdgId())==3 
						   || abs(genParticle->pdgId())==4 || abs(genParticle->pdgId())==5 
						   || abs(genParticle->pdgId())==21) )
						{
							// Algorithmic:
							if (genParticle->status() != 3)
							{
								deltaR = ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, genParticle->p4);
								if (deltaR<(settings.*GetDeltaRGenMatchingObjects)())
								{
									matching_algo_partons.push_back(*genParticle);
									if (std::abs(genParticle->pdgId()) == 5)
									{ 
										if (hardest_b_quark == NULL)
											hardest_b_quark = &(*genParticle);
										else if (genParticle->p4.Pt() > hardest_b_quark->p4.Pt())
											hardest_b_quark = &(*genParticle);
									}
									else if (std::abs(genParticle->pdgId()) == 4)
									{ 
										if (hardest_c_quark == NULL)
											hardest_c_quark = &(*genParticle);
										else if (genParticle->p4.Pt() > hardest_c_quark->p4.Pt())
											hardest_c_quark = &(*genParticle);
									}
									else if (hardest_parton == NULL)
										hardest_parton = &(*genParticle);
									else if (genParticle->p4.Pt() > hardest_parton->p4.Pt())
										hardest_parton = &(*genParticle);
								}
							}
							// Physics:
							else
							{
								deltaR = ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, genParticle->p4);
								if (deltaR<(settings.*GetDeltaRGenMatchingObjects)())
								{
									matching_phys_partons.push_back(*genParticle);
									hardest_phys_parton = &(*genParticle);
								}
							} 
						}
					}
					
					// ALGORITHMIC DEFINITION
					if (matching_algo_partons.size() == 1)	  // exactly one match
					{
						if((settings.*GetMatchingAlgorithmusObjects)()=="algorithmic")
						{
							if(hardest_b_quark)
								(product.*m_genMatchedObjects)[*validObject] = &(*hardest_b_quark);
							else if(hardest_c_quark)
								(product.*m_genMatchedObjects)[*validObject] = &(*hardest_c_quark);
							else
								(product.*m_genMatchedObjects)[*validObject] = &(*hardest_parton);
							objectMatched = true;
						}
					}
					else if (hardest_b_quark && hardest_b_quark->p4.Pt() > 0.)
					{
						if((settings.*GetMatchingAlgorithmusObjects)()=="algorithmic")
						{
							(product.*m_genMatchedObjects)[*validObject] = &(*hardest_b_quark);
							objectMatched = true;
						}
					}
					else if (hardest_c_quark && hardest_c_quark->p4.Pt() > 0.)
					{
						if((settings.*GetMatchingAlgorithmusObjects)()=="algorithmic")
						{
							(product.*m_genMatchedObjects)[*validObject] = &(*hardest_c_quark);
							objectMatched = true;
						}
					}
					else if (matching_algo_partons.size() != 0)
					{
						if((settings.*GetMatchingAlgorithmusObjects)()=="algorithmic")
						{
							(product.*m_genMatchedObjects)[*validObject] = &(*hardest_parton);
							objectMatched = true;
						}
					}
					
					// PHYSICS DEFINITION
					// flavour is only well defined if exactly ONE matching parton!
					if (matching_phys_partons.size() == 1)
					{
						if((settings.*GetMatchingAlgorithmusObjects)()=="physic")
						{
							(product.*m_genMatchedObjects)[*validObject] = &(*hardest_phys_parton);
							objectMatched = true;	
						}
						//calculate ratio only for Physics definition
						ratioGenMatched += 1./(product.*m_validObjects).size(); 
					}	

					// invalidate the object if the trigger has not matched
					if ((! objectMatched) && (settings.*GetInvalidateNonGenMatchingObjects)())
					{
						(product.*m_invalidObjects).push_back(*validObject);
						validObject = (product.*m_validObjects).erase(validObject);
					}
					else
					{
						++validObject;
					}
				}
				// preserve sorting of invalid objects
				if ((settings.*GetInvalidateNonGenMatchingObjects)())
				{
					std::sort((product.*m_invalidObjects).begin(), (product.*m_invalidObjects).end(),
					          [](TValidObject const* object1, TValidObject const* object2) -> bool
					          { return object1->p4.Pt() > object2->p4.Pt(); });
				}
			}
	  }
	  // matching e,m,t to gen taus
	  else
	  {
			assert(event.m_genTaus);
			
			if ((settings.*GetDeltaRGenMatchingObjects)() > 0.0)
			{
				// loop over all valid objects to check
				for (typename std::vector<TValidObject*>::iterator validObject = (product.*m_validObjects).begin();
					 validObject != (product.*m_validObjects).end();)
				{
					bool objectMatched = false;
					float deltaR = 0;
					
					// loop over all genTaus
					for (typename std::vector<KDataGenTau>::iterator genTau = event.m_genTaus->begin();
						 !objectMatched && genTau != event.m_genTaus->end();++genTau) 
					{
						// only use genTaus that will decay into comparable particles
						if (MatchDecayMode(*genTau,tauDecayMode))
						{
							deltaR = ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, genTau->p4_vis);
							if(deltaR<(settings.*GetDeltaRGenMatchingObjects)())
							{
								(product.*m_genMatchedObjects)[*validObject] = &(*genTau);
								ratioGenMatched += 1./(product.*m_validObjects).size();
								product.m_genMatchDeltaR = deltaR;
								objectMatched = true;
							}
							else product.m_genMatchDeltaR = DefaultValues::UndefinedFloat;
						}
						else product.m_genMatchDeltaR = DefaultValues::UndefinedFloat;
					}
					// invalidate the object if the trigger has not matched
					if ((! objectMatched) && (settings.*GetInvalidateNonGenMatchingObjects)())
					{
						(product.*m_invalidObjects).push_back(*validObject);
						validObject = (product.*m_validObjects).erase(validObject);
					}
					else
					{
						++validObject;
					}
				}
				// preserve sorting of invalid objects
				if ((settings.*GetInvalidateNonGenMatchingObjects)())
				{
					std::sort((product.*m_invalidObjects).begin(), (product.*m_invalidObjects).end(),
							  [](TValidObject const* object1, TValidObject const* object2) -> bool
							  { return object1->p4.Pt() > object2->p4.Pt(); });
				}
			}
			else product.m_genMatchDeltaR = DefaultValues::UndefinedFloat;
	  }
	  product.m_ratioGenMatched = ratioGenMatched;
	}
	
	virtual bool MatchDecayMode(KDataGenTau const &genTau, TauDecayMode tauDecayMode) const
	{
		bool decayModeMatched = false;
		switch(tauDecayMode) 
  		{	
				case TauDecayMode::NONE:
					LOG(ERROR) << "tauDecayMode == NONE"; //used for jets
					break;
				case TauDecayMode::E:
					if (genTau.isElectronicDecay())
						decayModeMatched = true;
					break;
				case TauDecayMode::M: 
					if (genTau.isMuonicDecay())
						decayModeMatched = true;
					break;
				case TauDecayMode::T:
					if (genTau.isHadronicDecay())
						decayModeMatched = true;
					break;
		};
		return decayModeMatched;
	}
	
private:
	std::map<TValidObject*, KGenParticle*> product_type::*m_genMatchedObjects; //changed to KGenParticle from const KDataLV
	std::vector<TValidObject*> product_type::*m_validObjects;
	std::vector<TValidObject*> product_type::*m_invalidObjects;
	TauDecayMode tauDecayMode;
	float (setting_type::*GetDeltaRGenMatchingObjects)(void) const;
	bool (setting_type::*GetInvalidateNonGenMatchingObjects)(void) const;
	std::string (setting_type::*GetMatchingAlgorithmusObjects)(void) const;
	
	std::map<size_t, std::vector<std::string> > m_objectTriggerFiltersByIndex;
	std::map<std::string, std::vector<std::string> > m_objectTriggerFiltersByHltName;

};


/** Producer for gen matched electrons
 *  Required config tags:
 *  - DeltaRGenMatchingElectrons (default provided)
 *  - InvalidateNonMatchingElectrons (default provided)
 */
class ElectronGenMatchingProducer: public GenMatchingProducerBase<KDataElectron>
{

public:
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;

	ElectronGenMatchingProducer();

};


/** Producer for gen matched muons
 *  Required config tags:
 *  - DeltaRGenMatchingMuons (default provided)
 *  - InvalidateNonMatchingMuons (default provided)
 */
class MuonGenMatchingProducer: public GenMatchingProducerBase<KDataMuon>
{

public:
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;
	
	MuonGenMatchingProducer();

};


/** Producer for gen matched taus
 *  Required config tags:
 *  - DeltaRGenMatchingTaus (default provided)
 *  - InvalidateNonMatchingTaus (default provided)
 */
class TauGenMatchingProducer: public GenMatchingProducerBase<KDataPFTau>
{

public:
	
		virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;
	
	TauGenMatchingProducer();

};


/** Producer for gen matched jets
 *  Required config tags:
 *  - DeltaRGenMatchingJets (default provided)
 *  - InvalidateNonMatchingJets (default provided) 
 *  - MatchingAlgorithmusJets (default provided)
 */
class JetGenMatchingProducer: public GenMatchingProducerBase<KDataPFJet>
{

public:
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;
	
	JetGenMatchingProducer();

};
