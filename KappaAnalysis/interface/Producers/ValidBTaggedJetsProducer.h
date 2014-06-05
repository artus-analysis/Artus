
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"


/**
   \brief Producer for valid b-tagged jets.

   Exploits the properties of b-tagged jets
*/
template<class TTypes>
class ValidBTaggedJetsProducer: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "valid_btagged_jets";
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		for (std::vector<KDataPFJet*>::iterator jet = product.m_validJets.begin();
				 jet != product.m_validJets.end(); ++jet)
			{
				bool validBJet = true;
				KDataPFTaggedJet* tjet = static_cast<KDataPFTaggedJet*>(*jet);

				float combinedSecondaryVertex = tjet->getTagger("CombinedSecondaryVertexBJetTags", event.m_taggermetadata);

				if (combinedSecondaryVertex < settings.GetBTaggedJetCombinedSecondaryVertexMediumWP() ||
					std::abs(tjet->p4.eta()) > settings.GetBTaggedJetAbsEtaCut()) {
					validBJet = false;
				}

				validBJet = validBJet && AdditionalCriteria(tjet, event, product, settings);

				if (validBJet)
					product.m_bTaggedJets.push_back(tjet);
				else
					product.m_nonBTaggedJets.push_back(tjet);
			}
	}


protected:

	virtual bool AdditionalCriteria(KDataPFTaggedJet* jet, event_type const& event,
	                                product_type& product, setting_type const& settings) const
	{
		bool validBJet = true;
		return validBJet;
	}

};
