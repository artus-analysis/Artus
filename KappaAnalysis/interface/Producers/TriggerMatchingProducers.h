
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

	// reccommended access to product.m_detailedTriggerMatched.... by using only the HLT paths where all configured filters match with the valid object
	static std::vector<std::string> GetHltNamesWhereAllFiltersMatched(
			std::map<std::string, std::map<std::string, std::vector<KLV*> > > const& detailedTriggerMatchedObjects
	) {
		std::vector<std::string> hltNames;
		
		for (std::pair<std::string, std::map<std::string, std::vector<KLV*> > > hltName : detailedTriggerMatchedObjects)
		{
			bool allFiltersMatched = true;
			for (std::pair<std::string, std::vector<KLV*> > filterName : hltName.second)
			{
				if (filterName.second.size() == 0)
				{
					allFiltersMatched = false;
				}
			}
			
			if (allFiltersMatched)
			{
				hltNames.push_back(hltName.first);
			}
		}
		
		return hltNames;
	}
	
	TriggerMatchingProducerBase(std::map<TValidObject*, KLV*> KappaProduct::*triggerMatchedObjects,
	                            std::map<TValidObject*, std::map<std::string, std::map<std::string, std::vector<KLV*> > > > KappaProduct::*detailedTriggerMatchedObjects,
	                            std::vector<TValidObject*> KappaProduct::*validObjects,
	                            std::vector<TValidObject*> KappaProduct::*invalidObjects,
	                            std::map<size_t, std::vector<std::string> > KappaProduct::*settingsObjectTriggerFiltersByIndex,
	                            std::map<std::string, std::vector<std::string> > KappaProduct::*settingsObjectTriggerFiltersByHltName,
	                            std::vector<std::string>& (KappaSettings::*GetObjectTriggerFilterNames)(void) const,
	                            float (KappaSettings::*GetDeltaRTriggerMatchingObjects)(void) const,
	                            bool (KappaSettings::*GetInvalidateNonMatchingObjects)(void) const) :
		m_triggerMatchedObjects(triggerMatchedObjects),
		m_detailedTriggerMatchedObjects(detailedTriggerMatchedObjects),
		m_validObjects(validObjects),
		m_invalidObjects(invalidObjects),
		m_settingsObjectTriggerFiltersByIndex(settingsObjectTriggerFiltersByIndex),
		m_settingsObjectTriggerFiltersByHltName(settingsObjectTriggerFiltersByHltName),
		GetObjectTriggerFilterNames(GetObjectTriggerFilterNames),
		GetDeltaRTriggerMatchingObjects(GetDeltaRTriggerMatchingObjects),
		GetInvalidateNonMatchingObjects(GetInvalidateNonMatchingObjects)
	{
	}

	virtual void Init(KappaSettings const& settings) override {
		KappaProducerBase::Init(settings);
		
		m_objectTriggerFiltersByIndexFromSettings = Utility::ParseMapTypes<size_t, std::string>(Utility::ParseVectorToMap((settings.*GetObjectTriggerFilterNames)()), m_objectTriggerFiltersByHltNameFromSettings);
	}

	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override
	{
		assert(event.m_triggerObjects);
		assert(event.m_triggerObjectMetadata);
		
		if ((product.*m_settingsObjectTriggerFiltersByIndex).empty())
		{
			(product.*m_settingsObjectTriggerFiltersByIndex).insert(m_objectTriggerFiltersByIndexFromSettings.begin(),
			                                                        m_objectTriggerFiltersByIndexFromSettings.end());
		}
		if ((product.*m_settingsObjectTriggerFiltersByHltName).empty())
		{
			(product.*m_settingsObjectTriggerFiltersByHltName).insert(m_objectTriggerFiltersByHltNameFromSettings.begin(),
			                                                          m_objectTriggerFiltersByHltNameFromSettings.end());
		}
		
		(product.*m_triggerMatchedObjects).clear();
		(product.*m_detailedTriggerMatchedObjects).clear();
		if ((! product.m_selectedHltNames.empty()) && ((settings.*GetDeltaRTriggerMatchingObjects)() > 0.0))
		{
			/*
			// TODO: remove debug output
			for (unsigned int iHlt = 0; iHlt < product.m_selectedHltNames.size(); iHlt++)
			{
				LOG(INFO) << product.m_selectedHltNames.at(iHlt) << " (" << product.m_selectedHltPositions.at(iHlt) << ")";
				for (size_t filterIndex = event.m_triggerObjectMetadata->getMinFilterIndex(product.m_selectedHltPositions.at(iHlt));
				     filterIndex <= event.m_triggerObjectMetadata->getMaxFilterIndex(product.m_selectedHltPositions.at(iHlt)); ++filterIndex)
				{
					LOG(INFO) << "\t" << event.m_triggerObjectMetadata->toFilter[filterIndex] << " (" << filterIndex << ")";
					for (std::vector<int>::const_iterator triggerObjectIndex = event.m_triggerObjects->toIdxFilter[filterIndex].begin();
					     triggerObjectIndex != event.m_triggerObjects->toIdxFilter[filterIndex].end();
					     ++triggerObjectIndex)
					{
						LOG(INFO) << "\t\tpt = " << event.m_triggerObjects->trgObjects[*triggerObjectIndex].p4.Pt();
					}
				}
			}
*/
			// loop over all valid objects to check
			for (typename std::vector<TValidObject*>::iterator validObject = (product.*m_validObjects).begin();
			     validObject != (product.*m_validObjects).end();)
			{
				bool objectMatched = false;
				std::vector<KLV*> matchedTriggerObjects;

				// loop over all fired HLT paths
				for (unsigned int iHlt = 0; iHlt < product.m_selectedHltNames.size(); iHlt++)
				{
					bool passAllFilters = true;

					// loop over all filters for the particulat HLT fired
					for (size_t filterIndex = event.m_triggerObjectMetadata->getMinFilterIndex(product.m_selectedHltPositions.at(iHlt));
					     (filterIndex < event.m_triggerObjectMetadata->getMaxFilterIndex(product.m_selectedHltPositions.at(iHlt)));
					     ++filterIndex)
					{
						bool hltMatched = false;
						bool filterMatched = false;

						// loop over the hlt names given in the config file
						for (std::map<std::string, std::vector<std::string>>::const_iterator objectTriggerFilterByHltName = (product.*m_settingsObjectTriggerFiltersByHltName).begin();
						     (!hltMatched) && (objectTriggerFilterByHltName != (product.*m_settingsObjectTriggerFiltersByHltName).end());
						     ++objectTriggerFilterByHltName)
						{
							// check that the hlt name given in the config matches the hlt which fired in the event
							if (boost::regex_search(product.m_selectedHltNames.at(iHlt),
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
									if (boost::regex_search(event.m_triggerObjectMetadata->toFilter[filterIndex],
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
						bool hasTriggerObjectMatch = false;
						std::vector<KLV*> matchedTriggerObjects;

						for (std::vector<int>::const_iterator triggerObjectIndex = event.m_triggerObjects->toIdxFilter[filterIndex].begin();
						     (triggerObjectIndex != event.m_triggerObjects->toIdxFilter[filterIndex].end());
						     ++triggerObjectIndex)
						{
							// check the matching
							if (ROOT::Math::VectorUtil::DeltaR(event.m_triggerObjects->trgObjects[*triggerObjectIndex].p4,
							                                   (*validObject)->p4) < (settings.*GetDeltaRTriggerMatchingObjects)())
							{
								hasTriggerObjectMatch = true;
								matchedTriggerObjects.push_back(&event.m_triggerObjects->trgObjects[*triggerObjectIndex]);
								
								// fill simple map: reco lepton --> trigger object
								(product.*m_triggerMatchedObjects)[*validObject] = &(event.m_triggerObjects->trgObjects[*triggerObjectIndex]);
								
								// fill detailed map: reco lepton --> HLT name --> filter name --> trigger object
								if ((product.*m_detailedTriggerMatchedObjects).count(*validObject) == 0)
								{
									(product.*m_detailedTriggerMatchedObjects)[*validObject] = std::map<std::string, std::map<std::string, std::vector<KLV*>>>();
								}
								if ((product.*m_detailedTriggerMatchedObjects)[*validObject].count(product.m_selectedHltNames.at(iHlt)) == 0)
								{
									(product.*m_detailedTriggerMatchedObjects)[*validObject][product.m_selectedHltNames.at(iHlt)] = std::map<std::string, std::vector<KLV*>>();
								}
								(product.*m_detailedTriggerMatchedObjects)[*validObject][product.m_selectedHltNames.at(iHlt)][event.m_triggerObjectMetadata->toFilter[filterIndex]] = matchedTriggerObjects;
							}
						}

						passAllFilters = passAllFilters && hasTriggerObjectMatch;

					} // end of loop over filter indexes

					objectMatched = objectMatched || passAllFilters;

				} // end of loop over fired HLT names

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

			} // end of loop over valid objects
			
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
	std::map<TValidObject*, KLV*> KappaProduct::*m_triggerMatchedObjects;
	std::map<TValidObject*, std::map<std::string, std::map<std::string, std::vector<KLV*> > > > KappaProduct::*m_detailedTriggerMatchedObjects;
	std::vector<TValidObject*> KappaProduct::*m_validObjects;
	std::vector<TValidObject*> KappaProduct::*m_invalidObjects;
	std::map<size_t, std::vector<std::string> > KappaProduct::*m_settingsObjectTriggerFiltersByIndex;
	std::map<std::string, std::vector<std::string> > KappaProduct::*m_settingsObjectTriggerFiltersByHltName;
	std::vector<std::string>& (KappaSettings::*GetObjectTriggerFilterNames)(void) const;
	float (KappaSettings::*GetDeltaRTriggerMatchingObjects)(void) const;
	bool (KappaSettings::*GetInvalidateNonMatchingObjects)(void) const;
	
	std::map<size_t, std::vector<std::string> > m_objectTriggerFiltersByIndexFromSettings;
	std::map<std::string, std::vector<std::string> > m_objectTriggerFiltersByHltNameFromSettings;

};


/** Producer for trigger matched electrons
 *  Required config tags:
 *  - DeltaRTriggerMatchingElectrons (default provided)
 *  - InvalidateNonMatchingElectrons (default provided)
 *  - ElectronTriggerFilterNames
 */
class ElectronTriggerMatchingProducer: public TriggerMatchingProducerBase<KElectron>
{

public:
	
	virtual std::string GetProducerId() const override;
	
	ElectronTriggerMatchingProducer();
	
	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;

};


/** Producer for trigger matched muons
 *  Required config tags:
 *  - DeltaRTriggerMatchingMuons (default provided)
 *  - InvalidateNonMatchingMuons (default provided)
 *  - MuonTriggerFilterNames
 */
class MuonTriggerMatchingProducer: public TriggerMatchingProducerBase<KMuon>
{

public:
	
	virtual std::string GetProducerId() const override;
	
	MuonTriggerMatchingProducer();
	
	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;

};


/** Producer for trigger matched taus
 *  Required config tags:
 *  - DeltaRTriggerMatchingTaus (default provided)
 *  - InvalidateNonMatchingTaus (default provided)
 *  - TauTriggerFilterNames
 */
class TauTriggerMatchingProducer: public TriggerMatchingProducerBase<KTau>
{

public:
	
	virtual std::string GetProducerId() const override;
	
	TauTriggerMatchingProducer();
	
	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;

};


/** Producer for trigger matched jets
 *  Required config tags:
 *  - DeltaRTriggerMatchingJets (default provided)
 *  - InvalidateNonMatchingJets (default provided)
 *  - JetTriggerFilterNames
 */
class JetTriggerMatchingProducer: public TriggerMatchingProducerBase<KBasicJet>
{

public:
	
	virtual std::string GetProducerId() const override;
	
	JetTriggerMatchingProducer();

};

