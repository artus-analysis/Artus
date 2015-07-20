
#include "Artus/KappaAnalysis/interface/Producers/GenParticleMatchingProducers.h"


std::string RecoJetGenParticleMatchingProducer::GetProducerId() const {
	return "RecoJetGenParticleMatchingProducer";
}

void RecoJetGenParticleMatchingProducer::Init(setting_type const& settings)
{
	KappaProducerBase::Init(settings);
	
	m_jetMatchingAlgorithm = ToJetMatchingAlgorithm(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetJetMatchingAlgorithm())));
	m_DeltaRMatchingRecoJetGenParticle = settings.GetDeltaRMatchingRecoJetGenParticle();
	m_InvalidateNonGenParticleMatchingRecoJets = settings.GetInvalidateNonGenParticleMatchingRecoJets();
	m_InvalidateGenParticleMatchingRecoJets = settings.GetInvalidateGenParticleMatchingRecoJets();
}

void RecoJetGenParticleMatchingProducer::Produce(event_type const& event, product_type& product,
                                                 setting_type const& settings) const
{
	assert(event.m_genParticles);
	
	if (m_DeltaRMatchingRecoJetGenParticle > 0.0f)
	{
		// loop over all valid objects (jets) to check
		for (std::vector<KBasicJet*>::iterator validJet = product.m_validJets.begin();
			 validJet != product.m_validJets.end();)
		{
			KGenParticle* matchedParticle = Match(event, product, settings, static_cast<KLV*>(*validJet));
			if (matchedParticle != NULL)
			{
				product.m_genParticleMatchedJets[*validJet] = matchedParticle;
			}

			// invalidate (non) matching jets if requested
			if (((matchedParticle == NULL) && m_InvalidateNonGenParticleMatchingRecoJets) ||
			    ((matchedParticle != NULL) && m_InvalidateGenParticleMatchingRecoJets))
			{
				product.m_invalidJets.push_back(*validJet);
				validJet = product.m_validJets.erase(validJet);
			}
			else
			{
				++validJet;
			}
		}
		
		// preserve sorting of invalid jets
		if (m_InvalidateNonGenParticleMatchingRecoJets || m_InvalidateGenParticleMatchingRecoJets)
		{
			std::sort(product.m_invalidJets.begin(), product.m_invalidJets.end(),
					  [](KBasicJet const* jet1, KBasicJet const* jet2) -> bool
					  { return jet1->p4.Pt() > jet2->p4.Pt(); });
		}
	}
}

// This is the actual reco jet gen particle matcher
KGenParticle* RecoJetGenParticleMatchingProducer::Match(event_type const& event, product_type const& product,
                                                        setting_type const& settings, KLV* const recoJet) const
{
	float deltaR = 0.0;
	size_t nMatchingAlgoPartons = 0;
	size_t nMatchingPhysPartons = 0;
	KGenParticle* hardestPhysParton = NULL;
	KGenParticle* hardestParton = NULL;
	KGenParticle* hardestBQuark = NULL;
	KGenParticle* hardestCQuark = NULL;

	// loop over all genParticles 
	for (std::vector<KGenParticle>::iterator genParticle = event.m_genParticles->begin();
	     genParticle != event.m_genParticles->end(); ++genParticle)
	{
		// only use genParticles with id 21, 1, -1, 2, -2, 3, -3, 4, -4, 5, -5
		if ((abs(genParticle->pdgId()) == 1) ||
		    (abs(genParticle->pdgId()) == 2) ||
		    (abs(genParticle->pdgId()) == 3) ||
		    (abs(genParticle->pdgId()) == 4) ||
		    (abs(genParticle->pdgId()) == 5) ||
		    (genParticle->pdgId()) == 21)
		{
			deltaR = ROOT::Math::VectorUtil::DeltaR((recoJet)->p4, genParticle->p4);
			if (deltaR < m_DeltaRMatchingRecoJetGenParticle)
			{
				// Algorithmic:
				if (genParticle->status() != settings.GetRecoJetMatchingGenParticleStatus())
				{
					++nMatchingAlgoPartons;
					if (std::abs(genParticle->pdgId()) == 5)
					{ 
						if (hardestBQuark == NULL)
						{
							hardestBQuark = &(*genParticle);
						}
						else if (genParticle->p4.Pt() > hardestBQuark->p4.Pt())
						{
							hardestBQuark = &(*genParticle);
						}
					}
					else if (std::abs(genParticle->pdgId()) == 4)
					{ 
						if (hardestCQuark == NULL)
						{
							hardestCQuark = &(*genParticle);
						}
						else if (genParticle->p4.Pt() > hardestCQuark->p4.Pt())
						{
							hardestCQuark = &(*genParticle);
						}
					}
					else if (hardestParton == NULL)
					{
						hardestParton = &(*genParticle);
					}
					else if (genParticle->p4.Pt() > hardestParton->p4.Pt())
					{
						hardestParton = &(*genParticle);
					}
				}

				// Physics:
				else
				{
					++nMatchingPhysPartons;
					hardestPhysParton = &(*genParticle);
				}
			} 
		}
	}

	// ALGORITHMIC DEFINITION
	if (nMatchingAlgoPartons)	  // exactly one match
	{
		if (m_jetMatchingAlgorithm == JetMatchingAlgorithm::ALGORITHMIC)
		{
			if(hardestBQuark)
			{
				return &(*hardestBQuark);
			}
			else if(hardestCQuark)
			{
				return &(*hardestCQuark);
			}
			else
			{
				return &(*hardestParton);
			}
		}
	}
	else if (hardestBQuark && hardestBQuark->p4.Pt() > 0.0f)
	{
		if (m_jetMatchingAlgorithm == JetMatchingAlgorithm::ALGORITHMIC)
		{
			return &(*hardestBQuark);
		}
	}
	else if (hardestCQuark && hardestCQuark->p4.Pt() > 0.0f)
	{
		if (m_jetMatchingAlgorithm == JetMatchingAlgorithm::ALGORITHMIC)
		{
			return &(*hardestCQuark);
		}
	}
	else if (nMatchingAlgoPartons)
	{
		if (m_jetMatchingAlgorithm == JetMatchingAlgorithm::ALGORITHMIC)
		{
			return &(*hardestParton);
		}
	}

	// PHYSICS DEFINITION
	// flavour is only well defined if exactly ONE matching parton!
	if (nMatchingPhysPartons == 1)
	{
		if (m_jetMatchingAlgorithm == JetMatchingAlgorithm::PHYSICS)
		{
			return &(*hardestPhysParton);
		}
	}
	return NULL;
}



std::string RecoElectronGenParticleMatchingProducer::GetProducerId() const {
	return "RecoElectronGenParticleMatchingProducer";
}

RecoElectronGenParticleMatchingProducer::RecoElectronGenParticleMatchingProducer() :
	RecoLeptonGenParticleMatchingProducerBase<KElectron>(&product_type::m_genParticleMatchedElectrons,
	                                                     &product_type::m_validElectrons,
	                                                     &product_type::m_invalidElectrons,
	                                                     &setting_type::GetRecoElectronMatchingGenParticlePdgIds,
	                                                     &setting_type::GetRecoElectronMatchingGenParticleStatus,
	                                                     &setting_type::GetDeltaRMatchingRecoElectronsGenParticle,
	                                                     &setting_type::GetInvalidateNonGenParticleMatchingRecoElectrons,
	                                                     &setting_type::GetInvalidateGenParticleMatchingRecoElectrons)
{
}


std::string RecoMuonGenParticleMatchingProducer::GetProducerId() const {
	return "RecoMuonGenParticleMatchingProducer";
}

RecoMuonGenParticleMatchingProducer::RecoMuonGenParticleMatchingProducer() :
	RecoLeptonGenParticleMatchingProducerBase<KMuon>(&product_type::m_genParticleMatchedMuons,
	                                                 &product_type::m_validMuons,
	                                                 &product_type::m_invalidMuons,
	                                                 &setting_type::GetRecoMuonMatchingGenParticlePdgIds,
	                                                 &setting_type::GetRecoMuonMatchingGenParticleStatus,
	                                                 &setting_type::GetDeltaRMatchingRecoMuonGenParticle,
	                                                 &setting_type::GetInvalidateNonGenParticleMatchingRecoMuons,
	                                                 &setting_type::GetInvalidateGenParticleMatchingRecoMuons)
{
}


std::string RecoTauGenParticleMatchingProducer::GetProducerId() const {
	return "RecoTauGenParticleMatchingProducer";
}

RecoTauGenParticleMatchingProducer::RecoTauGenParticleMatchingProducer() :
	RecoLeptonGenParticleMatchingProducerBase<KTau>(&product_type::m_genParticleMatchedTaus,
	                                                &product_type::m_validTaus,
	                                                &product_type::m_invalidTaus,
	                                                &setting_type::GetRecoTauMatchingGenParticlePdgIds,
	                                                &setting_type::GetRecoTauMatchingGenParticleStatus,
	                                                &setting_type::GetDeltaRMatchingRecoTauGenParticle,
	                                                &setting_type::GetInvalidateNonGenParticleMatchingRecoTaus,
	                                                &setting_type::GetInvalidateGenParticleMatchingRecoTaus)
{
}

