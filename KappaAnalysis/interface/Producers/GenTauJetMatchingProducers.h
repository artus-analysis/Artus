
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/** Abstract Producer class for trigger matching valid objects
 *
 *	Needs to run after the valid object producers.
 */
template<class TValidObject>
class GenTauJetMatchingProducerBase: public ProducerBase<KappaTypes>
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
	
	GenTauJetMatchingProducerBase(std::map<TValidObject*, KGenJet*> product_type::*genTauJetMatchedObjects,
	                           std::vector<TValidObject*> product_type::*validObjects,
	                           std::vector<TValidObject*> product_type::*invalidObjects,
	                           TauDecayMode tauDecayMode,
	                           float (setting_type::*GetDeltaRMatchingRecoObjectGenTauJet)(void) const,
	                           bool (setting_type::*GetInvalidateNonGenTauJetMatchingObjects)(void) const) :
		m_genTauJetMatchedObjects(genTauJetMatchedObjects),
		m_validObjects(validObjects),
		m_invalidObjects(invalidObjects),
		tauDecayMode(tauDecayMode),
		GetDeltaRMatchingRecoObjectGenTauJet(GetDeltaRMatchingRecoObjectGenTauJet),
		GetInvalidateNonGenTauJetMatchingObjects(GetInvalidateNonGenTauJetMatchingObjects)
	{
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE 
	{
		ProducerBase<KappaTypes>::Init(settings);
	}

	virtual void Produce(event_type const& event, product_type& product,
						 setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		assert(event.m_genTauJets);
		
		if ((settings.*GetDeltaRMatchingRecoObjectGenTauJet)() > 0.0)
		{
			// loop over all valid objects to check
			for (typename std::vector<TValidObject*>::iterator validObject = (product.*m_validObjects).begin();
				 validObject != (product.*m_validObjects).end();)
			{
				bool objectMatched = false;
				float deltaR = 0;
				
				// loop over all genTauJets
				for (typename std::vector<KGenJet>::iterator genTauJet = event.m_genTauJets->begin();
					 !objectMatched && genTauJet != event.m_genTauJets->end(); ++genTauJet) 
				{
					// TODO: maybe need to match to visible component of genTauJet
					// depends on setting for TauGenJetProducer.includeNeutrinos
					// PhysicsTools/JetMCAlgos/python/TauGenJets_cfi.py
					deltaR = ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, genTauJet->p4);
					if(deltaR<(settings.*GetDeltaRMatchingRecoObjectGenTauJet)())
					{
						(product.*m_genTauJetMatchedObjects)[*validObject] = &(*genTauJet);
						objectMatched = true;
					}
				}
				// invalidate the object if the trigger has not matched
				if ((! objectMatched) && (settings.*GetInvalidateNonGenTauJetMatchingObjects)())
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
			if ((settings.*GetInvalidateNonGenTauJetMatchingObjects)())
			{
				std::sort((product.*m_invalidObjects).begin(), (product.*m_invalidObjects).end(),
						  [](TValidObject const* object1, TValidObject const* object2) -> bool
						  { return object1->p4.Pt() > object2->p4.Pt(); });
			}
		}
	}
	
private:
	std::map<TValidObject*, KGenJet*> product_type::*m_genTauJetMatchedObjects; //changed to KGenParticle from const KDataLV
	std::vector<TValidObject*> product_type::*m_validObjects;
	std::vector<TValidObject*> product_type::*m_invalidObjects;
	TauDecayMode tauDecayMode;
	float (setting_type::*GetDeltaRMatchingRecoObjectGenTauJet)(void) const;
	bool (setting_type::*GetInvalidateNonGenTauJetMatchingObjects)(void) const;
	
	std::map<size_t, std::vector<std::string> > m_objectTriggerFiltersByIndex;
	std::map<std::string, std::vector<std::string> > m_objectTriggerFiltersByHltName;

};


/** Producer for gen matched electrons
 *  Required config tags:
 *  - DeltaRMatchingRecoElectronsGenTau (default provided)
 *  - InvalidateNonGenTauJetMatchingRecoElectrons (default provided)
 */
class RecoElectronGenTauJetMatchingProducer: public GenTauJetMatchingProducerBase<KElectron>
{

public:
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;

	RecoElectronGenTauJetMatchingProducer();

};


/** Producer for gen matched muons
 *  Required config tags:
 *  - DeltaRMatchingRecoMuonGenTau (default provided)
 *  - InvalidateNonGenTauJetMatchingRecoMuons (default provided)
 */
class RecoMuonGenTauJetMatchingProducer: public GenTauJetMatchingProducerBase<KMuon>
{

public:
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;
	
	RecoMuonGenTauJetMatchingProducer();

};


/** Producer for gen matched taus
 *  Required config tags:
 *  - DeltaRMatchingRecoTauGenTau (default provided)
 *  - InvalidateNonGenTauJetMatchingRecoTaus (default provided)
 */
class RecoTauGenTauJetMatchingProducer: public GenTauJetMatchingProducerBase<KTau>
{

public:
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;
	
	RecoTauGenTauJetMatchingProducer();

};

