
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
		
		if ((! product.m_selectedHltNames.empty()) && ((settings.*GetDeltaRTriggerMatchingObjects)() > 0.0))
		{
			bool hasHltAndFilterMatch = false;
			
			// loop over the hlt names given in the config file
			for (std::map<std::string, std::vector<std::string>>::const_iterator objectTriggerFilterByHltName = (product.*m_settingsObjectTriggerFiltersByHltName).begin();
			     objectTriggerFilterByHltName != (product.*m_settingsObjectTriggerFiltersByHltName).end();
			     ++objectTriggerFilterByHltName)
			{
				//LOG(DEBUG) << "objectTriggerFilterByHltName->first = " << objectTriggerFilterByHltName->first;
				
				// loop over all fired HLT paths
				for (unsigned int firedHltIndex = 0; firedHltIndex < product.m_selectedHltNames.size(); firedHltIndex++)
				{
					std::string firedHltName = product.m_selectedHltNames.at(firedHltIndex);
					int firedHltPosition = product.m_selectedHltPositions.at(firedHltIndex);
					//LOG(DEBUG) << "\tfiredFilterIndex, firedHltName, firedHltPosition = " << firedHltIndex << ", " << firedHltName << ", " << firedHltPosition;
					
					// check that the hlt name given in the config matches the hlt which fired in the event
					if (boost::regex_search(firedHltName, boost::regex(objectTriggerFilterByHltName->first, boost::regex::icase | boost::regex::extended)))
					{
						//LOG(DEBUG) << "\t\thltMatched";
						
						// loop over the filter regexp associated with the given hlt in the config
						for (std::vector<std::string>::const_iterator filterName = objectTriggerFilterByHltName->second.begin();
						     filterName != objectTriggerFilterByHltName->second.end();
						     ++filterName)
						{
							//LOG(DEBUG) << "\t\t\tfilterName = " << *filterName;
							
							// loop over all filters for the fired HLT
							for (size_t firedFilterIndex = event.m_triggerObjectMetadata->getMinFilterIndex(firedHltPosition);
							     firedFilterIndex < event.m_triggerObjectMetadata->getMaxFilterIndex(firedHltPosition);
							     ++firedFilterIndex)
							{
								std::string firedFilterName = event.m_triggerObjectMetadata->toFilter[firedFilterIndex];
								//LOG(DEBUG) << "\t\t\t\tfiredFilterIndex, firedFilterName = " << firedFilterIndex << ", " << firedFilterName;
								
								// check that the filter regexp given in the config matches the fired filter
								if (boost::regex_search(firedFilterName, boost::regex(*filterName, boost::regex::icase | boost::regex::extended)))
								{
									hasHltAndFilterMatch = true;
									//LOG(DEBUG) << "\t\t\t\t\tfilterMatched";
									
									// loop over all valid objects to check
									for (typename std::vector<TValidObject*>::iterator validObject = (product.*m_validObjects).begin();
									     validObject != (product.*m_validObjects).end(); ++validObject)
									{
										std::vector<KLV*> matchedTriggerObjects;
										//LOG(DEBUG) << "\t\t\t\t\t\tvalidObject = " << *validObject;
										
										// loop over all trigger objects for the fired filter
										for (std::vector<int>::const_iterator triggerObjectIndex = event.m_triggerObjects->toIdxFilter[firedFilterIndex].begin();
										     triggerObjectIndex != event.m_triggerObjects->toIdxFilter[firedFilterIndex].end();
										     ++triggerObjectIndex)
										{
											KLV triggerObject = event.m_triggerObjects->trgObjects.at(*triggerObjectIndex);
											//LOG(DEBUG) << "\t\t\t\t\t\t\ttriggerObjectIndex, triggerObject = " << *triggerObjectIndex << ", " << &triggerObject;
											
											// check the matching
											if (ROOT::Math::VectorUtil::DeltaR(triggerObject.p4, (*validObject)->p4) < (settings.*GetDeltaRTriggerMatchingObjects)())
											{
												//LOG(DEBUG) << "\t\t\t\t\t\t\t\tobjectMatched";
												
												matchedTriggerObjects.push_back(&triggerObject);
											}
										}
										
										if (matchedTriggerObjects.size() > 0)
										{
											(product.*m_triggerMatchedObjects)[*validObject] = matchedTriggerObjects.at(0);
											(product.*m_detailedTriggerMatchedObjects)[*validObject][firedHltName][firedFilterName] = matchedTriggerObjects;
										}
										else // require matched with ALL filters specified
										{
											if ((product.*m_triggerMatchedObjects).count(*validObject))
											{
												(product.*m_triggerMatchedObjects).erase(*validObject);
											}
											if ((product.*m_detailedTriggerMatchedObjects).count(*validObject))
											{
												(product.*m_detailedTriggerMatchedObjects).erase(*validObject);
											}
										}
									}
								}
							}
						}
					}
				}
			}
			
			// invalidate the object if the trigger has not matched
			if (hasHltAndFilterMatch && (settings.*GetInvalidateNonMatchingObjects)())
			{
				for (typename std::vector<TValidObject*>::iterator validObject = (product.*m_validObjects).begin(); validObject != (product.*m_validObjects).end();)
				{
					
					if ((product.*m_triggerMatchedObjects).count(*validObject) == 0)
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
				std::sort((product.*m_invalidObjects).begin(), (product.*m_invalidObjects).end(),
				          [](TValidObject const* object1, TValidObject const* object2) -> bool
				          { return object1->p4.Pt() > object2->p4.Pt(); });
			}
			
			/*
			// debug output
			LOG(DEBUG) << "";
			LOG(DEBUG) << "Result of trigger matching:";
			// works
			for (typename std::pair<TValidObject*, std::map<std::string, std::map<std::string, std::vector<KLV*> > > > validObject : (product.*m_detailedTriggerMatchedObjects))
			{
				LOG(DEBUG) << "Reco object = " << validObject.first << " (" << validObject.second.size() << ")";
				for (std::pair<std::string, std::map<std::string, std::vector<KLV*> > > hltName : validObject.second)
				{
					LOG(DEBUG) << "\tHLT name = " << hltName.first << " (" << hltName.second.size() << ")";
					for (std::pair<std::string, std::vector<KLV*> > filterName : hltName.second)
					{
						LOG(DEBUG) << "\t\tFilter name = " << filterName.first << " (" << filterName.second.size() << ")";
						for (KLV* triggerObject : filterName.second)
						{
							LOG(DEBUG) << "\t\t\tTrigger object = " << triggerObject;
						}
					}
				}
			}
			// does not work --> why?
			*//*for (typename std::map<TValidObject*, std::map<std::string, std::map<std::string, std::vector<KLV*> > > >::iterator validObject = (product.*m_detailedTriggerMatchedObjects).begin();
			     validObject != (product.*m_detailedTriggerMatchedObjects).end(); ++validObject)
			{
				LOG(DEBUG) << "Reco object = " << validObject->first << " (" << validObject->second.size() << ")";
				for (std::map<std::string, std::map<std::string, std::vector<KLV*> > >::iterator hltName = validObject->second.begin();
				     hltName != validObject->second.begin(); ++hltName)
				{
					LOG(DEBUG) << "\tHLT name = " << hltName->first << " (" << hltName->second.size() << ")";
					for (std::map<std::string, std::vector<KLV*> >::iterator filterName = hltName->second.begin();
					     filterName != hltName->second.begin(); ++filterName)
					{
						LOG(DEBUG) << "\tFilter name = " << filterName->first << " (" << filterName->second.size() << ")";
						for (std::vector<KLV*>::iterator triggerObject = filterName->second.begin();
						     triggerObject != filterName->second.begin(); ++triggerObject)
						{
							LOG(DEBUG) << "\tTrigger object = " << *triggerObject;
						}
					}
				}
			}*//*
			LOG(DEBUG) << "";
			*/
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

