
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"


/**
   \brief Trigger matching producer
   
   This producer looks for the vectors of valid electrons, muons, taus and (tagged) jets in the event
   and checks whether the four momentum vector lies inside a cone around the trigger objects.
   If so, the valid object is added to a list product.m_triggerMatched*
   
   This producer needs to run after the HltProducer/HltFilter and the Valid*Producer/Valid*Filter
   
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
	
	static KDataLV GetTriggerObject(event_type const& event, product_type const& product, size_t const& hltIndex)
	{
		return event.m_triggerObjects->trgObjects.at(hltIndex);
	}

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "trigger_matching";
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		assert(event.m_triggerObjects);
		
		if (! product.m_selectedHltName.empty())
		{
			//LOG(INFO) << product.m_selectedHltName << ", " << event.m_triggerObjects->toIdxHLT.at(product.m_selectedHltPosition).size();
			for (std::vector<size_t>::iterator hltIndex = event.m_triggerObjects->toIdxHLT.at(product.m_selectedHltPosition).begin();
				 hltIndex != event.m_triggerObjects->toIdxHLT.at(product.m_selectedHltPosition).end(); ++hltIndex)
			{
			
				if (product.m_validElectrons.size() != 0 && settings.GetDeltaRTriggerMatchingElectron() > 0.0)
				{
					MatchValidObjects(product.m_validElectrons, *hltIndex, settings.GetDeltaRTriggerMatchingElectron(),
					                  event, product, product.m_triggerMatchedElectrons);
				}
			
				if (product.m_validMuons.size() != 0 && settings.GetDeltaRTriggerMatchingMuon() > 0.0)
				{
					 MatchValidObjects(product.m_validMuons, *hltIndex, settings.GetDeltaRTriggerMatchingMuon(),
					                   event, product, product.m_triggerMatchedMuons);
				}
			
				if (product.m_validTaus.size() != 0 && settings.GetDeltaRTriggerMatchingTau() > 0.0)
				{
					MatchValidObjects(product.m_validTaus, *hltIndex, settings.GetDeltaRTriggerMatchingTau(),
					                  event, product, product.m_triggerMatchedTaus);
				}
			
				if (settings.GetDeltaRTriggerMatchingJet() > 0.0)
				{
					MatchValidObjects(product.m_validJets, *hltIndex, settings.GetDeltaRTriggerMatchingJet(),
					                  event, product, product.m_triggerMatchedJets);
				}
			}
		}
	}


private:
	
	template<class TValidObject>
	void MatchValidObjects(std::vector<TValidObject*> validObjects, size_t hltIndex, float deltaRThreshold,
	                       event_type const& event, product_type& product,
	                       std::map<TValidObject*, size_t>& matchedValidObjects) const
	{
		KDataLV triggerObject = TriggerMatchingProducer<TTypes>::GetTriggerObject(event, product, hltIndex);

		for (typename std::vector<TValidObject*>::iterator validObject = validObjects.begin();
			 validObject != validObjects.end(); validObject++)
		{
			if (ROOT::Math::VectorUtil::DeltaR(triggerObject.p4, (*validObject)->p4) < deltaRThreshold)
			{
				matchedValidObjects[*validObject] = hltIndex;
			}
		}
	}
};

