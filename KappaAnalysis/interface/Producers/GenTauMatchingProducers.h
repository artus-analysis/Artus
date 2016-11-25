
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
class GenTauMatchingProducerBase: public ProducerBase<KappaTypes>
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
	
	GenTauMatchingProducerBase(std::map<TValidObject*, KGenTau*> product_type::*genTauMatchedObjects, //changed to KGenParticle from const KDataLV
	                           std::vector<TValidObject*> product_type::*validObjects,
	                           std::vector<TValidObject*> product_type::*invalidObjects,
	                           TauDecayMode tauDecayMode,
	                           float (setting_type::*GetDeltaRMatchingRecoObjectGenTau)(void) const,
	                           bool (setting_type::*GetInvalidateNonGenTauMatchingObjects)(void) const,
	                           bool (setting_type::*GetInvalidateGenTauMatchingObjects)(void) const) :
		m_genTauMatchedObjects(genTauMatchedObjects),
		m_validObjects(validObjects),
		m_invalidObjects(invalidObjects),
		tauDecayMode(tauDecayMode),
		GetDeltaRMatchingRecoObjectGenTau(GetDeltaRMatchingRecoObjectGenTau),
		GetInvalidateNonGenTauMatchingObjects(GetInvalidateNonGenTauMatchingObjects),
		GetInvalidateGenTauMatchingObjects(GetInvalidateGenTauMatchingObjects)
	{
	}

	void Init(setting_type const& settings) override 
	{
		ProducerBase<KappaTypes>::Init(settings);
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("ratioGenTauMatched", [](event_type const & event, product_type const & product)
		{
			return product.m_ratioGenTauMatched;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genTauMatchTauDeltaR", [](event_type const & event, product_type const & product)
		{
			return product.m_genTauMatchDeltaR;
		});
	}

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings) const override
	{
		double ratioGenTauMatched = 0;
		
		assert(event.m_genTaus);
		
		if ((settings.*GetDeltaRMatchingRecoObjectGenTau)() > 0.0f)
		{
			// loop over all valid objects to check
			for (typename std::vector<TValidObject*>::iterator validObject = (product.*m_validObjects).begin();
				 validObject != (product.*m_validObjects).end();)
			{
				bool objectMatched = false;
				float deltaR = 0;
				float deltaRmin = std::numeric_limits<float>::max();
				
				// loop over all genTaus
				for (typename std::vector<KGenTau>::iterator genTau = event.m_genTaus->begin(); 
					genTau != event.m_genTaus->end(); ++genTau) 
				{
					// only use genTaus that will decay into comparable particles
					if (MatchDecayMode(*genTau,tauDecayMode))
					{
						deltaR = static_cast<float>(ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, genTau->visible.p4));
						if(deltaR<(settings.*GetDeltaRMatchingRecoObjectGenTau)() && deltaR<deltaRmin)
						{
							(product.*m_genTauMatchedObjects)[*validObject] = &(*genTau);
							product.m_genTauMatchedLeptons[*validObject] = &(*genTau);
							ratioGenTauMatched += 1.0 / (product.*m_validObjects).size();
							product.m_genTauMatchDeltaR = deltaR;
							deltaRmin = deltaR;
							objectMatched = true;
							//LOG(INFO) << this->GetProducerId() << " (event " << event.m_eventInfo->nEvent << "): " << (*validObject)->p4 << " --> " << genTau->visible.p4;
						}
						else product.m_genTauMatchDeltaR = DefaultValues::UndefinedFloat;
					}
					else product.m_genTauMatchDeltaR = DefaultValues::UndefinedFloat;
				}
				// invalidate the object if it has not matched
				if (((! objectMatched) && (settings.*GetInvalidateNonGenTauMatchingObjects)()) ||
				    (objectMatched && (settings.*GetInvalidateGenTauMatchingObjects)()))
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
			if ((settings.*GetInvalidateNonGenTauMatchingObjects)() || (settings.*GetInvalidateGenTauMatchingObjects)())
			{
				std::sort((product.*m_invalidObjects).begin(), (product.*m_invalidObjects).end(),
						  [](TValidObject const* object1, TValidObject const* object2) -> bool
						  { return object1->p4.Pt() > object2->p4.Pt(); });
			}
		}
		else
		{
			product.m_genTauMatchDeltaR = DefaultValues::UndefinedFloat;
		}
		product.m_ratioGenTauMatched = ratioGenTauMatched;
	}
	
	virtual bool MatchDecayMode(KGenTau const &genTau, TauDecayMode tauDecayMode) const
	{
		bool decayModeMatched = (((tauDecayMode == TauDecayMode::E) && genTau.isElectronicDecay()) ||
		                         ((tauDecayMode == TauDecayMode::M) && genTau.isMuonicDecay()) ||
		                         ((tauDecayMode == TauDecayMode::T) && genTau.isHadronicDecay()));
		return decayModeMatched;
	}
	
private:
	std::map<TValidObject*, KGenTau*> product_type::*m_genTauMatchedObjects; //changed to KGenParticle from const KDataLV
	std::vector<TValidObject*> product_type::*m_validObjects;
	std::vector<TValidObject*> product_type::*m_invalidObjects;
	TauDecayMode tauDecayMode;
	float (setting_type::*GetDeltaRMatchingRecoObjectGenTau)(void) const;
	bool (setting_type::*GetInvalidateNonGenTauMatchingObjects)(void) const;
	bool (setting_type::*GetInvalidateGenTauMatchingObjects)(void) const;
	
	std::map<size_t, std::vector<std::string> > m_objectTriggerFiltersByIndex;
	std::map<std::string, std::vector<std::string> > m_objectTriggerFiltersByHltName;

};


/** Producer for gen matched electrons
 *  Required config tags:
 *  - DeltaRMatchingRecoElectronsGenTau (default provided)
 *  - InvalidateNonGenTauMatchingRecoElectrons (default provided)
 *  - InvalidateGenTauMatchingRecoElectrons (default provided)
 */
class RecoElectronGenTauMatchingProducer: public GenTauMatchingProducerBase<KElectron>
{

public:
	RecoElectronGenTauMatchingProducer();

};


/** Producer for gen matched muons
 *  Required config tags:
 *  - DeltaRMatchingRecoMuonGenTau (default provided)
 *  - InvalidateNonGenTauMatchingRecoMuons (default provided)
 *  - InvalidateGenTauMatchingRecoMuons (default provided)
 */
class RecoMuonGenTauMatchingProducer: public GenTauMatchingProducerBase<KMuon>
{

public:
	RecoMuonGenTauMatchingProducer();

};


/** Producer for gen matched taus
 *  Required config tags:
 *  - DeltaRMatchingRecoTauGenTau (default provided)
 *  - InvalidateNonGenTauMatchingRecoTaus (default provided)
 *  - InvalidateGenTauMatchingRecoTaus (default provided)
 */
class RecoTauGenTauMatchingProducer: public GenTauMatchingProducerBase<KTau>
{

public:
	RecoTauGenTauMatchingProducer();

};

