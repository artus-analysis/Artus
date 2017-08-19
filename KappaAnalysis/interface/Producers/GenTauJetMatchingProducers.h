
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
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

	GenTauJetMatchingProducerBase(std::map<TValidObject*, KGenJet*> product_type::*genTauJetMatchedObjects,
	                           std::vector<TValidObject*> product_type::*validObjects,
	                           std::vector<TValidObject*> product_type::*invalidObjects,
	                           TauDecayMode tauDecayMode,
	                           float (setting_type::*GetDeltaRMatchingRecoObjectGenTauJet)(void) const,
	                           bool (setting_type::*GetInvalidateNonGenTauJetMatchingObjects)(void) const,
				   bool (setting_type::*GetInvalidateGenTauJetMatchingObjects)(void) const) :
		m_genTauJetMatchedObjects(genTauJetMatchedObjects),
		m_validObjects(validObjects),
		m_invalidObjects(invalidObjects),
		tauDecayMode(tauDecayMode),
		GetDeltaRMatchingRecoObjectGenTauJet(GetDeltaRMatchingRecoObjectGenTauJet),
		GetInvalidateNonGenTauJetMatchingObjects(GetInvalidateNonGenTauJetMatchingObjects),
		GetInvalidateGenTauJetMatchingObjects(GetInvalidateGenTauJetMatchingObjects)
	{
	}

	void Init(setting_type const& settings, metadata_type& metadata) override 
	{
		ProducerBase<KappaTypes>::Init(settings, metadata);
	}

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override
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
				float deltaRmin = std::numeric_limits<float>::max();
				
				// loop over all genTauJets
				for (typename std::vector<KGenJet>::iterator genTauJet = event.m_genTauJets->begin();
					genTauJet != event.m_genTauJets->end(); ++genTauJet) 
				{
					// TODO: maybe need to match to visible component of genTauJet
					// depends on setting for TauGenJetProducer.includeNeutrinos
					// PhysicsTools/JetMCAlgos/python/TauGenJets_cfi.py
					deltaR = ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, genTauJet->p4);
					if(deltaR<(settings.*GetDeltaRMatchingRecoObjectGenTauJet)() && deltaR<deltaRmin)
					{
						(product.*m_genTauJetMatchedObjects)[*validObject] = &(*genTauJet);
						deltaRmin = deltaR;
						objectMatched = true;
						//LOG(INFO) << this->GetProducerId() << " (event " << event.m_eventInfo->nEvent << "): " << (*validObject)->p4 << " --> " << genTauJet->p4;
					}
				}
				// invalidate the object if it has not matched
				if (((! objectMatched) && (settings.*GetInvalidateNonGenTauJetMatchingObjects)()) ||
				    (objectMatched && (settings.*GetInvalidateGenTauJetMatchingObjects)()))
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
			if ((settings.*GetInvalidateNonGenTauJetMatchingObjects)() || (settings.*GetInvalidateGenTauJetMatchingObjects)())
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
	bool (setting_type::*GetInvalidateGenTauJetMatchingObjects)(void) const;
	
	std::map<size_t, std::vector<std::string> > m_objectTriggerFiltersByIndex;
	std::map<std::string, std::vector<std::string> > m_objectTriggerFiltersByHltName;

};


/** Producer for gen matched electrons
 *  Required config tags:
 *  - DeltaRMatchingRecoElectronsGenTau (default provided)
 *  - InvalidateNonGenTauJetMatchingRecoElectrons (default provided)
 *  - InvalidateGenTauJetMatchingRecoElectrons (default provided)
 */
class RecoElectronGenTauJetMatchingProducer: public GenTauJetMatchingProducerBase<KElectron>
{

public:
	
	std::string GetProducerId() const override;

	RecoElectronGenTauJetMatchingProducer();

};


/** Producer for gen matched muons
 *  Required config tags:
 *  - DeltaRMatchingRecoMuonGenTau (default provided)
 *  - InvalidateNonGenTauJetMatchingRecoMuons (default provided)
 *  - InvalidateGenTauJetMatchingRecoMuons (default provided)
 */
class RecoMuonGenTauJetMatchingProducer: public GenTauJetMatchingProducerBase<KMuon>
{

public:
	
	std::string GetProducerId() const override;
	
	RecoMuonGenTauJetMatchingProducer();

};


/** Producer for gen matched taus
 *  Required config tags:
 *  - DeltaRMatchingRecoTauGenTau (default provided)
 *  - InvalidateNonGenTauJetMatchingRecoTaus (default provided)
 *  - InvalidateGenTauJetMatchingRecoTaus (default provided)
 */
class RecoTauGenTauJetMatchingProducer: public GenTauJetMatchingProducerBase<KTau>
{

public:
	
	std::string GetProducerId() const override;
	
	RecoTauGenTauJetMatchingProducer();

};

