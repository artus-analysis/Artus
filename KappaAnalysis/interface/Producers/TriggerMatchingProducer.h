
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
	typedef typename TTypes::setting_type setting_type;
	
	static bool LeptonHasMatched(event_type const& event, product_type const& product, KLepton* lepton, size_t const& index)
	{
		auto positionElectron = std::find(product.m_triggerMatchedElectrons.begin(), product.m_triggerMatchedElectrons.end(), lepton);
		if (positionElectron != product.m_triggerMatchedElectrons.end())
		{
			index = std::distance(product.m_triggerMatchedElectrons.begin(), positionElectron);
			return true;
		}
		
		auto positionMuon = std::find(product.m_triggerMatchedMuons.begin(), product.m_triggerMatchedMuons.end(), lepton);
		if (positionMuon != product.m_triggerMatchedMuons.end())
		{
			index = std::distance(product.m_triggerMatchedMuons.begin(), positionMuon);
			return true;
		}
		
		auto positionTau = std::find(product.m_triggerMatchedTaus.begin(), product.m_triggerMatchedTaus.end(), lepton);
		if (positionTau != product.m_triggerMatchedTaus.end())
		{
			index = std::distance(product.m_triggerMatchedTaus.begin(), positionTau);
			return true;
		}
		
		return false;
	}
	
	static bool JetHasMatched(event_type const& event, product_type const& product, KDataPFJet* jet, size_t const& index)
	{
		auto positionJet = std::find(product.m_triggerMatchedJets.begin(), product.m_triggerMatchedJets.end(), jet);
		if (positionJet != product.m_triggerMatchedJets.end())
		{
			index = std::distance(product.m_triggerMatchedJets.begin(), positionJet);
			return true;
		}
		
		auto positionTaggedJet = std::find(product.m_triggerMatchedTaggedJets.begin(), product.m_triggerMatchedTaggedJets.end(), jet);
		if (positionTaggedJet != product.m_triggerMatchedTaggedJets.end())
		{
			index = std::distance(product.m_triggerMatchedTaggedJets.begin(), positionTaggedJet);
			return true;
		}
		
		return false;
	}

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "trigger_matching";
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		assert(event.m_triggerObjects);
		
		if (! product.selectedHltName.empty())
		{
			size_t hltPosition = product.m_hltInfo->getHLTPosition(product.selectedHltName);
			for (std::vector<size_t>::iterator hltIndex = event.m_triggerObjects->toIdxHLT.at(hltPosition).begin();
				 hltIndex != event.m_triggerObjects->toIdxHLT.at(hltPosition).end(); ++hltIndex)
			{
				KDataLV triggerObject = event.m_triggerObjects->trgObjects.at(*hltIndex);
			
				if (event.m_electrons && settings.GetDeltaRTriggerMatchingElectron() > 0.0)
				{
					product.m_triggerMatchedElectrons = MatchRecoObjects(event.m_electrons, triggerObject, settings.GetDeltaRTriggerMatchingElectron());
				}
			
				if (event.m_muons && settings.GetDeltaRTriggerMatchingMuon() > 0.0)
				{
					product.m_triggerMatchedMuons = MatchRecoObjects(event.m_muons, triggerObject, settings.GetDeltaRTriggerMatchingMuon());
				}
			
				if (event.m_taus && settings.GetDeltaRTriggerMatchingTau() > 0.0)
				{
					product.m_triggerMatchedTaus = MatchRecoObjects(event.m_taus, triggerObject, settings.GetDeltaRTriggerMatchingTau());
				}
			
				if (settings.GetDeltaRTriggerMatchingJet() > 0.0)
				{
					if (event.m_jets)
					{
						product.m_triggerMatchedJets = MatchRecoObjects(event.m_jets, triggerObject, settings.GetDeltaRTriggerMatchingJet());
					}
					if (event.m_tjets)
					{
						product.m_triggerMatchedTaggedJets = MatchRecoObjects(event.m_tjets, triggerObject, settings.GetDeltaRTriggerMatchingJet());
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

