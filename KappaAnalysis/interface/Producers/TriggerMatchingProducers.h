
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
	
	TriggerMatchingProducerBase(std::map<TValidObject*, KDataLV*> product_type::*triggerMatchedObjects,
	                            std::vector<TValidObject*> product_type::*validObjects,
	                            std::vector<TValidObject*> product_type::*invalidObjects,
	                            std::vector<std::string>& (setting_type::*GetObjectTriggerFilterNames)(void) const,
	                            float (setting_type::*GetDeltaRTriggerMatchingObjects)(void) const,
	                            bool (setting_type::*GetInvalidateNonMatchingObjects)(void) const) :
		m_triggerMatchedObjects(triggerMatchedObjects),
		m_validObjects(validObjects),
		m_invalidObjects(invalidObjects),
		GetObjectTriggerFilterNames(GetObjectTriggerFilterNames),
		GetDeltaRTriggerMatchingObjects(GetDeltaRTriggerMatchingObjects),
		GetInvalidateNonMatchingObjects(GetInvalidateNonMatchingObjects)
	{
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		ProducerBase<TTypes>::Init(settings);
		
		objectTriggerFiltersByIndex = Utility::ParseMapTypes<size_t, std::string>(Utility::ParseVectorToMap((settings.*GetObjectTriggerFilterNames)()), objectTriggerFiltersByHltName);
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		assert(event.m_triggerObjects);
		
		if ((! product.m_selectedHltName.empty()) && ((settings.*GetDeltaRTriggerMatchingObjects)() > 0.0))
		{
/*
			// TODO: remove debug output
			LOG(INFO) << product.m_selectedHltName << " (" << product.m_selectedHltPosition << ")";
			for (size_t filterIndex = event.m_triggerInfos->getMinFilterIndex(product.m_selectedHltPosition);
			     filterIndex <= event.m_triggerInfos->getMaxFilterIndex(product.m_selectedHltPosition); ++filterIndex)
			{
				LOG(INFO) << "\t" << event.m_triggerInfos->toFilter[filterIndex] << " (" << filterIndex << ")";
				for (std::vector<int>::const_iterator triggerObjectIndex = event.m_triggerObjects->toIdxFilter[filterIndex].begin();
				     triggerObjectIndex != event.m_triggerObjects->toIdxFilter[filterIndex].end();
				     ++triggerObjectIndex)
				{
					LOG(INFO) << "\t\tpt = " << event.m_triggerObjects->trgObjects[*triggerObjectIndex].p4.Pt();
				}
			}
*/
			// loop over all valid objects to check
			for (typename std::vector<TValidObject*>::iterator validObject = (product.*m_validObjects).begin();
			     validObject != (product.*m_validObjects).end();)
			{
				bool objectMatched = false;
				
				// loop over all filters
				for (size_t filterIndex = event.m_triggerInfos->getMinFilterIndex(product.m_selectedHltPosition);
					(! objectMatched) &&
					(filterIndex <= event.m_triggerInfos->getMaxFilterIndex(product.m_selectedHltPosition));
				     ++filterIndex)
				{
					bool hltMatched = false;
					bool filterMatched = false;

					// loop over the hlt names given in the config file
					for (std::map<std::string, std::vector<std::string>>::const_iterator objectTriggerFilterByHltName = objectTriggerFiltersByHltName.begin();
					(!hltMatched) &&
					(objectTriggerFilterByHltName != objectTriggerFiltersByHltName.end()); ++objectTriggerFilterByHltName)
					{
						// check that the hlt name given in the config matches the hlt which fired in the event
						if (boost::regex_search(product.m_selectedHltName, boost::regex(objectTriggerFilterByHltName->first, boost::regex::icase | boost::regex::extended))) {

							hltMatched = true;
							std::vector<std::string> objectTriggerFilters = objectTriggerFilterByHltName->second;

							// loop over the filter regexp associated with the given hlt in the config
							for (std::vector<std::string>::const_iterator filterName = objectTriggerFilters.begin();
								(!filterMatched) && 
								(filterName != objectTriggerFilters.end());
								++filterName) {

								// check that the filter regexp matches the filter
								if (boost::regex_search(event.m_triggerInfos->toFilter[filterIndex], boost::regex(*filterName, boost::regex::icase | boost::regex::extended)))
									filterMatched = true;
							}
						}
					}

					// proceed with object matching only if the filter matches the filter regexp
					if (!filterMatched)
						continue;

					// loop over all trigger objects for this filter
					for (std::vector<int>::const_iterator triggerObjectIndex = event.m_triggerObjects->toIdxFilter[filterIndex].begin();
					(! objectMatched) &&
					(triggerObjectIndex != event.m_triggerObjects->toIdxFilter[filterIndex].end());
					++triggerObjectIndex)
					{
						// check the matching
						if (ROOT::Math::VectorUtil::DeltaR(event.m_triggerObjects->trgObjects[*triggerObjectIndex].p4,
						                                   (*validObject)->p4) < (settings.*GetDeltaRTriggerMatchingObjects)())
						{
							(product.*m_triggerMatchedObjects)[*validObject] = &(event.m_triggerObjects->trgObjects[*triggerObjectIndex]);
							objectMatched = true;
						}
					}
				}
				
				// invalidate the object if the trigger has not matched
				if ((! objectMatched) && (settings.*GetInvalidateNonMatchingObjects)())
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
	std::map<size_t, std::vector<std::string> > objectTriggerFiltersByIndex;
	std::map<std::string, std::vector<std::string> > objectTriggerFiltersByHltName;
	
	std::map<TValidObject*, KDataLV*> product_type::*m_triggerMatchedObjects;
	std::vector<TValidObject*> product_type::*m_validObjects;
	std::vector<TValidObject*> product_type::*m_invalidObjects;
	std::vector<std::string>& (setting_type::*GetObjectTriggerFilterNames)(void) const;
	float (setting_type::*GetDeltaRTriggerMatchingObjects)(void) const;
	bool (setting_type::*GetInvalidateNonMatchingObjects)(void) const;

};


/** Producer for trigger matched electrons
 *  Required config tags:
 *  - DeltaRTriggerMatchingElectrons (default provided)
 *  - InvalidateNonMatchingElectrons (default provided)
 *  - ElectronTriggerFilterNames
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
		                                                   &setting_type::GetElectronTriggerFilterNames,
		                                                   &setting_type::GetDeltaRTriggerMatchingElectrons,
		                                                   &setting_type::GetInvalidateNonMatchingElectrons)
	{
	}

};


/** Producer for trigger matched muons
 *  Required config tags:
 *  - DeltaRTriggerMatchingMuons (default provided)
 *  - InvalidateNonMatchingMuons (default provided)
 *  - MuonTriggerFilterNames
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
		                                               &setting_type::GetMuonTriggerFilterNames,
		                                               &setting_type::GetDeltaRTriggerMatchingMuons,
		                                               &setting_type::GetInvalidateNonMatchingMuons)
	{
	}

};


/** Producer for trigger matched taus
 *  Required config tags:
 *  - DeltaRTriggerMatchingTaus (default provided)
 *  - InvalidateNonMatchingTaus (default provided)
 *  - TauTriggerFilterNames
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
		                                                &setting_type::GetTauTriggerFilterNames,
		                                                &setting_type::GetDeltaRTriggerMatchingTaus,
		                                                &setting_type::GetInvalidateNonMatchingTaus)
	{
	}

};


/** Producer for trigger matched jets
 *  Required config tags:
 *  - DeltaRTriggerMatchingJets (default provided)
 *  - InvalidateNonMatchingJets (default provided)
 *  - JetTriggerFilterNames
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
		                                                &setting_type::GetJetTriggerFilterNames,
		                                                &setting_type::GetDeltaRTriggerMatchingJets,
		                                                &setting_type::GetInvalidateNonMatchingJets)
	{
	}

};

