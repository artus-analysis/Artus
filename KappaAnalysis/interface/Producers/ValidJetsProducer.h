
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
		for (KDataPFJets::iterator pfJet = event.m_pfJets->begin();
		     pfJet != event.m_pfJets->end(); ++pfJet)
		{
			bool good_jet = true;

			/* TODO
			// Muon isolation DeltaR > 0.5
			if (muonIso)
			{
				float dr1, dr2;
				dr1 = 99999.0f;
				dr2 = 99999.0f;

				if (product.HasValidZ())
				{
					dr1 = ROOT::Math::VectorUtil::DeltaR(pfJet->p4,
														 product.GetValidMuons().at(0).p4);
					dr2 = ROOT::Math::VectorUtil::DeltaR(pfJet->p4,
														 product.GetValidMuons().at(1).p4);
				}
				good_jet = good_jet && (dr1 > 0.5) && (dr2 > 0.5);
			}
			*/

			// JetID
			// https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
			// PFJets, all eta
			good_jet = good_jet
					   && pfJet->neutralHadFraction + pfJet->HFHadFraction < 0.99
					   && pfJet->neutralEMFraction < 0.99
					   && pfJet->nConst > 1;
			// PFJets, |eta| < 2.4 (tracker)
			if (std::abs(pfJet->p4.eta()) < 2.4)
			{
				good_jet = good_jet
						   && pfJet->chargedHadFraction > 0.0
						   && pfJet->nCharged > 0
						   && pfJet->chargedEMFraction < 0.99;
			}

			/* TODO
			if (globalSettings.Global()->GetVetoPileupJets())
			{
				bool puID = static_cast<KDataPFTaggedJet*>pfJet->getpuJetID("PUJetIDFullMedium", event.m_taggermetadata);
				good_jet = good_jet && puID;
			}
			*/

			if (good_jet)
				product.m_validJets.push_back(&(*pfJet));
			else
				product.m_invalidJets.push_back(&(*pfJet));
		}

		return true;
	}
	
	//bool tagged;
	//bool muonIso;
};

