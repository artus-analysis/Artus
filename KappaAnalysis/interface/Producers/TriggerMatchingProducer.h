
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"


/**
   \brief Trigger matching producer
   
   This producer looks for the vectors of electrons, muons, taus and (tagged) jets in the event
   and checks whether the the four momentum vector lies inside a cone around trigger objects.
   If so, the reconstructed object is added to a list product.m_triggerMatched*
   
   This producer needs to run after the HltProducer/HltFilter.
   
   Required config tags
	- DeltaRTriggerMatchingElectron (default given)
	- DeltaRTriggerMatchingMuon (default given)
	- DeltaRTriggerMatchingTau (default given)
	- DeltaRTriggerMatchingJet (default given)
*/

template<class TTypes>
class TriggerMatchingProducer: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "trigger_matching";
	}

	virtual void InitGlobal(global_setting_type const& globalSettings) ARTUS_CPP11_OVERRIDE {
		ProducerBase<TTypes>::InitGlobal(globalSettings);
		
		deltaRTriggerMatchingElectron = globalSettings.GetDeltaRTriggerMatchingElectron();
		deltaRTriggerMatchingMuon = globalSettings.GetDeltaRTriggerMatchingMuon();
		deltaRTriggerMatchingTau = globalSettings.GetDeltaRTriggerMatchingTau();
		deltaRTriggerMatchingJet = globalSettings.GetDeltaRTriggerMatchingJet();
	}

	virtual void InitLocal(setting_type const& settings) ARTUS_CPP11_OVERRIDE {
		ProducerBase<TTypes>::InitLocal(settings);
		
		deltaRTriggerMatchingElectron = settings.GetDeltaRTriggerMatchingElectron();
		deltaRTriggerMatchingMuon = settings.GetDeltaRTriggerMatchingMuon();
		deltaRTriggerMatchingTau = settings.GetDeltaRTriggerMatchingTau();
		deltaRTriggerMatchingJet = settings.GetDeltaRTriggerMatchingJet();
	}

	virtual void ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const ARTUS_CPP11_OVERRIDE
	{
		Produce(event, product);
	}

	virtual void ProduceLocal(event_type const& event,
	                          product_type& product,
	                          setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		Produce(event, product);
	}


protected:
	float deltaRTriggerMatchingElectron = 0.0;
	float deltaRTriggerMatchingMuon = 0.0;
	float deltaRTriggerMatchingTau = 0.0;
	float deltaRTriggerMatchingJet = 0.0;

	// function that lets this producer work as both a global and a local producer
	virtual void Produce(event_type const& event, product_type& product) const
	{
		assert(event.m_triggerObjects);
		
		if (! product.selectedHltName.empty())
		{
			size_t hltPosition = product.m_hltInfo->getHLTPosition(product.selectedHltName);
			for (std::vector<size_t>::iterator hltIndex = event.m_triggerObjects->toIdxHLT.at(hltPosition).begin();
				 hltIndex != event.m_triggerObjects->toIdxHLT.at(hltPosition).end(); ++hltIndex)
			{
				KDataLV triggerObject = event.m_triggerObjects->trgObjects.at(*hltIndex);
			
				if (event.m_electrons && deltaRTriggerMatchingElectron > 0.0)
				{
					product.m_triggerMatchedElectrons = MatchRecoObjects(event.m_electrons, triggerObject, deltaRTriggerMatchingElectron);
				}
			
				if (event.m_muons && deltaRTriggerMatchingMuon > 0.0)
				{
					product.m_triggerMatchedMuons = MatchRecoObjects(event.m_muons, triggerObject, deltaRTriggerMatchingMuon);
				}
			
				if (event.m_taus && deltaRTriggerMatchingTau > 0.0)
				{
					product.m_triggerMatchedTaus = MatchRecoObjects(event.m_taus, triggerObject, deltaRTriggerMatchingTau);
				}
			
				if (deltaRTriggerMatchingJet > 0.0)
				{
					if (event.m_jets)
					{
						product.m_triggerMatchedJets = MatchRecoObjects(event.m_jets, triggerObject, deltaRTriggerMatchingJet);
					}
					if (event.m_tjets)
					{
						product.m_triggerMatchedTaggedJets = MatchRecoObjects(event.m_tjets, triggerObject, deltaRTriggerMatchingJet);
					}
				}
			}
		}
	}


private:
	
	template<class TRecoObject>
	std::vector<TRecoObject*> MatchRecoObjects(std::vector<TRecoObject>* recoObjects, KDataLV const& triggerObject, float deltaRThreshold) const
	{
		std::vector<TRecoObject*> matchedRecoObjects;
		
		for (typename std::vector<TRecoObject>::iterator recoObject = recoObjects->begin();
			 recoObject != recoObjects->end(); recoObject++)
		{
			if (ROOT::Math::VectorUtil::DeltaR(triggerObject.p4, recoObject->p4) < deltaRThreshold)
			{
				matchedRecoObjects.push_back(&(*recoObject));
			}
		}
		
		return matchedRecoObjects;
	}
};

