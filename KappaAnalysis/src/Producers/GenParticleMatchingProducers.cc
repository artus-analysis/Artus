
#include "Artus/Utility/interface/DefaultValues.h"

#include "Artus/KappaAnalysis/interface/Producers/GenParticleMatchingProducers.h"


std::string RecoJetGenParticleMatchingProducer::GetProducerId() const {
	return "RecoJetGenParticleMatchingProducer";
}

void RecoJetGenParticleMatchingProducer::Init(setting_type const& settings)
{
	KappaProducerBase::Init(settings);
	
	jetMatchingAlgorithm = ToJetMatchingAlgorithm(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetJetMatchingAlgorithm())));
}

void RecoJetGenParticleMatchingProducer::Produce(event_type const& event, product_type& product,
                                                 setting_type const& settings) const
{
	assert(event.m_genParticles);
	
	if (settings.GetDeltaRMatchingRecoJetGenParticle() > 0.0)
	{
		// loop over all valid objects (jets) to check
		for (std::vector<KBasicJet*>::iterator validJet = product.m_validJets.begin();
			 validJet != product.m_validJets.end();)
		{
			bool jetMatched = false;
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
				    (abs(genParticle->pdgId()) == 21))
				{
					deltaR = ROOT::Math::VectorUtil::DeltaR((*validJet)->p4, genParticle->p4);
					if (deltaR < settings.GetDeltaRMatchingRecoJetGenParticle())
					{
						// Algorithmic:
						if (genParticle->status() != 3)
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
				if (jetMatchingAlgorithm == JetMatchingAlgorithm::ALGORITHMIC)
				{
					if(hardestBQuark)
					{
						product.m_genParticleMatchedJets[*validJet] = &(*hardestBQuark);
					}
					else if(hardestCQuark)
					{
						product.m_genParticleMatchedJets[*validJet] = &(*hardestCQuark);
					}
					else
					{
						product.m_genParticleMatchedJets[*validJet] = &(*hardestParton);
					}
					jetMatched = true;
				}
			}
			else if (hardestBQuark && hardestBQuark->p4.Pt() > 0.0)
			{
				if (jetMatchingAlgorithm == JetMatchingAlgorithm::ALGORITHMIC)
				{
					product.m_genParticleMatchedJets[*validJet] = &(*hardestBQuark);
					jetMatched = true;
				}
			}
			else if (hardestCQuark && hardestCQuark->p4.Pt() > 0.0)
			{
				if (jetMatchingAlgorithm == JetMatchingAlgorithm::ALGORITHMIC)
				{
					product.m_genParticleMatchedJets[*validJet] = &(*hardestCQuark);
					jetMatched = true;
				}
			}
			else if (nMatchingAlgoPartons)
			{
				if (jetMatchingAlgorithm == JetMatchingAlgorithm::ALGORITHMIC)
				{
					product.m_genParticleMatchedJets[*validJet] = &(*hardestParton);
					jetMatched = true;
				}
			}
			
			// PHYSICS DEFINITION
			// flavour is only well defined if exactly ONE matching parton!
			if (nMatchingPhysPartons == 1)
			{
				if (jetMatchingAlgorithm == JetMatchingAlgorithm::ALGORITHMIC)
				{
					product.m_genParticleMatchedJets[*validJet] = &(*hardestPhysParton);
					jetMatched = true;
				}
			}

			// invalidate not matching jets
			if ((! jetMatched) && settings.GetInvalidateNonGenParticleMatchingRecoJets())
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
		if (settings.GetInvalidateNonGenParticleMatchingRecoJets())
		{
			std::sort(product.m_invalidJets.begin(), product.m_invalidJets.end(),
					  [](KBasicJet const* jet1, KBasicJet const* jet2) -> bool
					  { return jet1->p4.Pt() > jet2->p4.Pt(); });
		}
	}
}



std::string RecoElectronGenParticleMatchingProducer::GetProducerId() const {
	return "RecoElectronGenParticleMatchingProducer";
}

RecoElectronGenParticleMatchingProducer::RecoElectronGenParticleMatchingProducer() :
	RecoLeptonGenParticleMatchingProducerBase<KElectron>(&product_type::m_genParticleMatchedElectrons,
	                                                         &product_type::m_validElectrons,
	                                                         &product_type::m_invalidElectrons,
	                                                         DefaultValues::pdgIdElectron,
	                                                         &setting_type::GetDeltaRMatchingRecoElectronsGenParticle,
	                                                         &setting_type::GetInvalidateNonGenParticleMatchingRecoElectrons)
{
}


std::string RecoMuonGenParticleMatchingProducer::GetProducerId() const {
	return "RecoMuonGenParticleMatchingProducer";
}

RecoMuonGenParticleMatchingProducer::RecoMuonGenParticleMatchingProducer() :
	RecoLeptonGenParticleMatchingProducerBase<KMuon>(&product_type::m_genParticleMatchedMuons,
	                                                     &product_type::m_validMuons,
	                                                     &product_type::m_invalidMuons,
	                                                     DefaultValues::pdgIdMuon,
	                                                     &setting_type::GetDeltaRMatchingRecoMuonGenParticle,
	                                                     &setting_type::GetInvalidateNonGenParticleMatchingRecoMuons)
{
}


std::string RecoTauGenParticleMatchingProducer::GetProducerId() const {
	return "RecoTauGenParticleMatchingProducer";
}

RecoTauGenParticleMatchingProducer::RecoTauGenParticleMatchingProducer() :
	RecoLeptonGenParticleMatchingProducerBase<KTau>(&product_type::m_genParticleMatchedTaus,
	                                                      &product_type::m_validTaus,
	                                                      &product_type::m_invalidTaus,
	                                                      DefaultValues::pdgIdTau,
	                                                      &setting_type::GetDeltaRMatchingRecoTauGenParticle,
	                                                      &setting_type::GetInvalidateNonGenParticleMatchingRecoTaus)
{
}

