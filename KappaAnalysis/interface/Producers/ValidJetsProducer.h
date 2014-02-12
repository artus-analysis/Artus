
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"


/** Select only valid jets.

    The jets are valid if the pass these criteria
    1. muon isolation
    1. jetID
*/
template<class TTypes>
class ValidJetsProducer: public GlobalProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;

	virtual std::string GetProducerId() {
		return "valid_jets";
	}

	virtual bool ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const
	{
		for (KDataPFJets::iterator jet = event.m_jets->begin();
		     jet != event.m_jets->end(); ++jet)
		{
			bool validJet = true;

			/* TODO
			// Muon isolation DeltaR > 0.5
			if (muonIso)
			{
				float dr1, dr2;
				dr1 = 99999.0f;
				dr2 = 99999.0f;

				if (product.HasValidZ())
				{
					dr1 = ROOT::Math::VectorUtil::DeltaR(jet->p4,
														 product.GetValidMuons().at(0).p4);
					dr2 = ROOT::Math::VectorUtil::DeltaR(jet->p4,
														 product.GetValidMuons().at(1).p4);
				}
				validJet = validJet && (dr1 > 0.5) && (dr2 > 0.5);
			}
			*/

			// JetID
			// https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
			// jets, all eta
			validJet = validJet
					   && jet->neutralHadFraction + jet->HFHadFraction < 0.99
					   && jet->neutralEMFraction < 0.99
					   && jet->nConst > 1;
			// jets, |eta| < 2.4 (tracker)
			if (std::abs(jet->p4.eta()) < 2.4)
			{
				validJet = validJet
						   && jet->chargedHadFraction > 0.0
						   && jet->nCharged > 0
						   && jet->chargedEMFraction < 0.99;
			}

			/* TODO
			if (globalSettings.Global()->GetVetoPileupJets())
			{
				bool puID = static_cast<KDataPFTaggedJet*>jet->getpuJetID("PUJetIDFullMedium", event.m_taggermetadata);
				validJet = validJet && puID;
			}
			*/

			if (validJet)
				product.m_validJets.push_back(&(*jet));
			else
				product.m_invalidJets.push_back(&(*jet));
		}

		return true;
	}
	
	//bool tagged;
	//bool muonIso;
};

