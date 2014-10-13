
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/** Abstract Producer class for trigger matching valid objects
 *
 *	Needs to run after the valid object producers.
 */
template<class TValidObject>
class TriggerMatchingProducerBase: public KappaProducerBase
{

public:
	
	TriggerMatchingProducerBase(std::map<TValidObject*, KDataLV*> KappaProduct::*triggerMatchedObjects,
	                            std::vector<TValidObject*> KappaProduct::*validObjects,
	                            std::vector<TValidObject*> KappaProduct::*invalidObjects,
	                            std::vector<std::string>& (KappaSettings::*GetObjectTriggerFilterNames)(void) const,
	                            float (KappaSettings::*GetDeltaRTriggerMatchingObjects)(void) const,
	                            bool (KappaSettings::*GetInvalidateNonMatchingObjects)(void) const) :
		m_triggerMatchedObjects(triggerMatchedObjects),
		m_validObjects(validObjects),
		m_invalidObjects(invalidObjects),
		GetObjectTriggerFilterNames(GetObjectTriggerFilterNames),
		GetDeltaRTriggerMatchingObjects(GetDeltaRTriggerMatchingObjects),
		GetInvalidateNonMatchingObjects(GetInvalidateNonMatchingObjects)
	{
	}

	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE {
		KappaProducerBase::Init(settings);
		
		m_objectTriggerFiltersByIndex = Utility::ParseMapTypes<size_t, std::string>(Utility::ParseVectorToMap((settings.*GetObjectTriggerFilterNames)()), m_objectTriggerFiltersByHltName);
	}

	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE
	{
		assert(event.m_triggerObjects);
		assert(event.m_triggerInfos);
		
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
					(filterIndex < event.m_triggerInfos->getMaxFilterIndex(product.m_selectedHltPosition));
				     ++filterIndex)
				{
					bool hltMatched = false;
					bool filterMatched = false;

					// loop over the hlt names given in the config file
					for (std::map<std::string, std::vector<std::string>>::const_iterator objectTriggerFilterByHltName = m_objectTriggerFiltersByHltName.begin();
					     (!hltMatched) && (objectTriggerFilterByHltName != m_objectTriggerFiltersByHltName.end());
					     ++objectTriggerFilterByHltName)
					{
						// check that the hlt name given in the config matches the hlt which fired in the event
						if (boost::regex_search(product.m_selectedHltName,
						                        boost::regex(objectTriggerFilterByHltName->first, boost::regex::icase | boost::regex::extended)))
						{

							hltMatched = true;
							std::vector<std::string> objectTriggerFilters = objectTriggerFilterByHltName->second;

							// loop over the filter regexp associated with the given hlt in the config
							for (std::vector<std::string>::const_iterator filterName = objectTriggerFilters.begin();
								 (!filterMatched) && (filterName != objectTriggerFilters.end());
								 ++filterName)
							{

								// check that the filter regexp matches the filter
								if (boost::regex_search(event.m_triggerInfos->toFilter[filterIndex],
								                        boost::regex(*filterName, boost::regex::icase | boost::regex::extended)))
								{
									filterMatched = true;
								}
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
	std::map<TValidObject*, KDataLV*> KappaProduct::*m_triggerMatchedObjects;
	std::vector<TValidObject*> KappaProduct::*m_validObjects;
	std::vector<TValidObject*> KappaProduct::*m_invalidObjects;
	std::vector<std::string>& (KappaSettings::*GetObjectTriggerFilterNames)(void) const;
	float (KappaSettings::*GetDeltaRTriggerMatchingObjects)(void) const;
	bool (KappaSettings::*GetInvalidateNonMatchingObjects)(void) const;
	
	std::map<size_t, std::vector<std::string> > m_objectTriggerFiltersByIndex;
	std::map<std::string, std::vector<std::string> > m_objectTriggerFiltersByHltName;

};


/** Producer for trigger matched electrons
 *  Required config tags:
 *  - DeltaRTriggerMatchingElectrons (default provided)
 *  - InvalidateNonMatchingElectrons (default provided)
 *  - ElectronTriggerFilterNames
 */
class ElectronTriggerMatchingProducer: public TriggerMatchingProducerBase<KDataElectron>
{

public:
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;
	
	ElectronTriggerMatchingProducer();

};


/** Producer for trigger matched muons
 *  Required config tags:
 *  - DeltaRTriggerMatchingMuons (default provided)
 *  - InvalidateNonMatchingMuons (default provided)
 *  - MuonTriggerFilterNames
 */
class MuonTriggerMatchingProducer: public TriggerMatchingProducerBase<KDataMuon>
{

public:
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;
	
	MuonTriggerMatchingProducer();

};


/** Producer for trigger matched taus
 *  Required config tags:
 *  - DeltaRTriggerMatchingTaus (default provided)
 *  - InvalidateNonMatchingTaus (default provided)
 *  - TauTriggerFilterNames
 */
class TauTriggerMatchingProducer: public TriggerMatchingProducerBase<KDataPFTau>
{

public:
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;
	
	TauTriggerMatchingProducer();

};


/** Producer for trigger matched jets
 *  Required config tags:
 *  - DeltaRTriggerMatchingJets (default provided)
 *  - InvalidateNonMatchingJets (default provided)
 *  - JetTriggerFilterNames
 */
class JetTriggerMatchingProducer: public TriggerMatchingProducerBase<KDataPFJet>
{

public:
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;
	
	JetTriggerMatchingProducer();

};

