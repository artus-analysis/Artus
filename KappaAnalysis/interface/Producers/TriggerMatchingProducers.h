
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"


/** Abstract Producer class for trigger matching valid objects
 *
 *	Needs to run after the valid object producers.
 */
template<class TTypes, class TValidObject>
class TriggerMatchingProducerBase: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	static KDataLV GetTriggerObject(event_type const& event, product_type const& product, size_t const& hltIndex)
	{
		return event.m_triggerObjects->trgObjects.at(hltIndex);
	}
	
	TriggerMatchingProducerBase(std::map<TValidObject*, size_t> product_type::*triggerMatchedObjects,
	                            std::vector<TValidObject*> product_type::*validObjects,
	                            std::vector<TValidObject*> product_type::*invalidObjects,
	                            float (setting_type::*GetDeltaRTriggerMatchingObjects)(void) const,
	                            bool (setting_type::*GetInvalidateNonMatchingObjects)(void) const) :
		m_triggerMatchedObjects(triggerMatchedObjects),
		m_validObjects(validObjects),
		m_invalidObjects(invalidObjects),
		GetDeltaRTriggerMatchingObjects(GetDeltaRTriggerMatchingObjects),
		GetInvalidateNonMatchingObjects(GetInvalidateNonMatchingObjects)
	{
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		assert(event.m_triggerObjects);
		
		if ((! product.m_selectedHltName.empty()) && ((settings.*GetDeltaRTriggerMatchingObjects)() > 0.0))
		{
			// loop over all valid objects to check
			for (typename std::vector<TValidObject*>::iterator validObject = (product.*m_validObjects).begin();
			     validObject != (product.*m_validObjects).end();)
			{
				//LOG(INFO) << product.m_selectedHltName << ", " << event.m_triggerObjects->toIdxHLT.at(product.m_selectedHltPosition).size();
				
				// loop over all trigger objects found in the trigger selected for this event
				bool matched = false;
				for (std::vector<size_t>::iterator hltIndex = event.m_triggerObjects->toIdxHLT.at(product.m_selectedHltPosition).begin();
				     (! matched) && (hltIndex != event.m_triggerObjects->toIdxHLT.at(product.m_selectedHltPosition).end()); ++hltIndex)
				{
					// check matching
					if (ROOT::Math::VectorUtil::DeltaR(TriggerMatchingProducerBase<TTypes, TValidObject>::GetTriggerObject(event, product, *hltIndex).p4,
					                                   (*validObject)->p4) < (settings.*GetDeltaRTriggerMatchingObjects)())
					{
						(product.*m_triggerMatchedObjects)[*validObject] = *hltIndex;
						matched = true;
					}
				}
				
				// invalidate objects if trigger has not matched
				if ((! matched) && (settings.*GetInvalidateNonMatchingObjects)())
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
			if ((settings.*GetInvalidateNonMatchingObjects)())
			{
				std::sort((product.*m_invalidObjects).begin(), (product.*m_invalidObjects).end(),
				          [](TValidObject const* object1, TValidObject const* object2) -> bool
				          { return object1->p4.Pt() > object2->p4.Pt(); });
			}
		}
	}


private:
	std::map<TValidObject*, size_t> product_type::*m_triggerMatchedObjects;
	std::vector<TValidObject*> product_type::*m_validObjects;
	std::vector<TValidObject*> product_type::*m_invalidObjects;
	float (setting_type::*GetDeltaRTriggerMatchingObjects)(void) const;
	bool (setting_type::*GetInvalidateNonMatchingObjects)(void) const;

};


/** Producer for trigger matched electrons
 *  Required config tags:
 *  - DeltaRTriggerMatchingElectrons (default provided)
 *  - InvalidateNonMatchingElectrons (default provided)
 */
template<class TTypes>
class ElectronTriggerMatchingProducer: public TriggerMatchingProducerBase<TTypes, KDataElectron>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "electron_trigger_matching";
	}
	
	ElectronTriggerMatchingProducer() :
		TriggerMatchingProducerBase<TTypes, KDataElectron>(&product_type::m_triggerMatchedElectrons,
		                                                   &product_type::m_validElectrons,
		                                                   &product_type::m_invalidElectrons,
		                                                   &setting_type::GetDeltaRTriggerMatchingElectrons,
		                                                   &setting_type::GetInvalidateNonMatchingElectrons)
	{
	}

};


/** Producer for trigger matched muons
 *  Required config tags:
 *  - DeltaRTriggerMatchingMuons (default provided)
 *  - InvalidateNonMatchingMuons (default provided)
 */
template<class TTypes>
class MuonTriggerMatchingProducer: public TriggerMatchingProducerBase<TTypes, KDataMuon>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "muon_trigger_matching";
	}
	
	MuonTriggerMatchingProducer() :
		TriggerMatchingProducerBase<TTypes, KDataMuon>(&product_type::m_triggerMatchedMuons,
		                                               &product_type::m_validMuons,
		                                               &product_type::m_invalidMuons,
		                                               &setting_type::GetDeltaRTriggerMatchingMuons,
		                                               &setting_type::GetInvalidateNonMatchingMuons)
	{
	}

};


/** Producer for trigger matched taus
 *  Required config tags:
 *  - DeltaRTriggerMatchingTaus (default provided)
 *  - InvalidateNonMatchingTaus (default provided)
 */
template<class TTypes>
class TauTriggerMatchingProducer: public TriggerMatchingProducerBase<TTypes, KDataPFTau>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "tau_trigger_matching";
	}
	
	TauTriggerMatchingProducer() :
		TriggerMatchingProducerBase<TTypes, KDataPFTau>(&product_type::m_triggerMatchedTaus,
		                                                &product_type::m_validTaus,
		                                                &product_type::m_invalidTaus,
		                                                &setting_type::GetDeltaRTriggerMatchingTaus,
		                                                &setting_type::GetInvalidateNonMatchingTaus)
	{
	}

};


/** Producer for trigger matched jets
 *  Required config tags:
 *  - DeltaRTriggerMatchingJets (default provided)
 *  - InvalidateNonMatchingJets (default provided)
 */
template<class TTypes>
class JetTriggerMatchingProducer: public TriggerMatchingProducerBase<TTypes, KDataPFJet>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "jet_trigger_matching";
	}
	
	JetTriggerMatchingProducer() :
		TriggerMatchingProducerBase<TTypes, KDataPFJet>(&product_type::m_triggerMatchedJets,
		                                                &product_type::m_validJets,
		                                                &product_type::m_invalidJets,
		                                                &setting_type::GetDeltaRTriggerMatchingJets,
		                                                &setting_type::GetInvalidateNonMatchingJets)
	{
	}

};

