
#include "Artus/KappaAnalysis/interface/Utility/GeneratorInfo.h"

KappaEnumTypes::GenMatchingCode GeneratorInfo::GetGenMatchingCode(KGenParticle* genParticle)
{
	int pdgId = std::abs(genParticle->pdgId);
	
	if (pdgId == 11 && genParticle->p4.Pt() > 8. && genParticle->isPrompt())
	{
		return KappaEnumTypes::GenMatchingCode::IS_ELE_PROMPT;
	}
	else if (pdgId == 13 && genParticle->p4.Pt() > 8. && genParticle->isPrompt())
	{
		return KappaEnumTypes::GenMatchingCode::IS_MUON_PROMPT;
	}
	else if (pdgId == 11 && genParticle->p4.Pt() > 8. && genParticle->isDirectPromptTauDecayProduct())
	{
		return KappaEnumTypes::GenMatchingCode::IS_ELE_FROM_TAU;
	}
	else if (pdgId == 13 && genParticle->p4.Pt() > 8. && genParticle->isDirectPromptTauDecayProduct())
	{
		return KappaEnumTypes::GenMatchingCode::IS_MUON_FROM_TAU;
	}
	else if (pdgId == 15 && GetVisibleLV(genParticle)->Pt() > 15.)
	{
		return KappaEnumTypes::GenMatchingCode::IS_TAU_HAD_DECAY;
	}
	else
	{
		return KappaEnumTypes::GenMatchingCode::IS_FAKE;
	}
}

RMFLV* GeneratorInfo::GetVisibleLV(KGenParticle* genParticle)
{
	RMFLV* visibleLV = nullptr;
	if (genParticle)
	{
		KGenTau* genTau = dynamic_cast<KGenTau*>(genParticle);
		if (genTau)
		{
			visibleLV = &(genTau->visible.p4);
		}
		else
		{
			visibleLV = &(genParticle->p4);
		}
	}
	return visibleLV;
}

KGenParticle* GeneratorInfo::GetGenMatchedParticle(
		KLepton* lepton,
		std::map<KLepton*, KGenParticle*> const& leptonGenParticleMap,
		std::map<KLepton*, KGenTau*> const& leptonGenTauMap
)
{
	KGenParticle* defaultGenParticle = nullptr;
	KGenParticle* genParticle = SafeMap::GetWithDefault(leptonGenParticleMap, lepton, defaultGenParticle);
	
	KGenTau* defaultGenTau = nullptr;
	KGenTau* genTau = SafeMap::GetWithDefault(leptonGenTauMap, lepton, defaultGenTau);
	
	float deltaRTauGenTau = (genTau ? ROOT::Math::VectorUtil::DeltaR(lepton->p4, genTau->visible.p4) : std::numeric_limits<float>::max());
	float deltaRTauGenParticle = (genParticle ? ROOT::Math::VectorUtil::DeltaR(lepton->p4, genParticle->p4) : std::numeric_limits<float>::max());
	
	if (deltaRTauGenParticle <= deltaRTauGenTau)
	{
		return genParticle;
	}
	else
	{
		return genTau;
	}
}

// This matching algorithm is used by the Wisconsin group and was provided by Cecile.
KappaEnumTypes::GenMatchingCode GeneratorInfo::GetGenMatchingCodeUW(
		KappaTypes::event_type const& event,
		KLepton* lepton
)
{
	if(event.m_genParticles && event.m_genParticles->size() > 0)
	{
		LOG(DEBUG) << "*************** START UW GEN MATCHING CODE ***************";
		LOG(DEBUG) << "Reco lepton p4: " << lepton->p4;
		KGenParticle closest = event.m_genParticles->at(0);
		double closestDR = 999;
		for (typename std::vector<KGenParticle>::iterator genParticle = event.m_genParticles->begin();
			genParticle != event.m_genParticles->end(); ++genParticle)
		{
			// https://twiki.cern.ch/twiki/bin/viewauth/CMS/HiggsToTauTauWorking2017#MC_Matching
			// check first whether there is a gen e/mu (prompt or tau decay product) closer than delta R < 0.2 with pt > 8 GeV
			if ( (genParticle->p4.Pt() > 8.) && (std::abs(genParticle->pdgId) == 11 || std::abs(genParticle->pdgId) == 13) && (genParticle->isPrompt() || genParticle->isDirectPromptTauDecayProduct()) )
			{
				double tmpDR = ROOT::Math::VectorUtil::DeltaR(lepton->p4, genParticle->p4);
				if (tmpDR < closestDR)
				{
					closest = *genParticle;
					closestDR = tmpDR;
				}
			}
		}
		LOG(DEBUG) << "found closest e/mu with smallest dR = " << closestDR;
		int pdgId = std::abs(closest.pdgId);
		// build "gen-tau jet" from visible gen tau decay products
		LOG(DEBUG) << "BuildGenTausUW: Build 'gen-tau jet' from visible decay products:";
		std::vector<RMFLV> genTaus = BuildGenTausUW(event);
		for(auto genTau : genTaus)
		{
			double gentauDR = ROOT::Math::VectorUtil::DeltaR(lepton->p4, genTau);
			// check if gen-tau jets are closer than e/mu
			if (gentauDR < 0.2 && genTau.Pt() > 15. && gentauDR < closestDR)
			{
				LOG(DEBUG) << "GenMatchingCode: IS_TAU_HAD_DECAY, smallest dR = " << gentauDR;
				return KappaEnumTypes::GenMatchingCode::IS_TAU_HAD_DECAY;
			}
		}
		if (closestDR < 0.2)
		{
			if (pdgId == 11 && closest.isPrompt())
			{
				LOG(DEBUG) << "GenMatchingCode: IS_ELE_PROMPT";
				return KappaEnumTypes::GenMatchingCode::IS_ELE_PROMPT;
			}
			else if (pdgId == 13 && closest.isPrompt())
			{
				LOG(DEBUG) << "GenMatchingCode: IS_MUON_PROMPT";
				return KappaEnumTypes::GenMatchingCode::IS_MUON_PROMPT;
			}
			else if (pdgId == 11 && closest.isDirectPromptTauDecayProduct())
			{
				LOG(DEBUG) << "GenMatchingCode: IS_ELE_FROM_TAU";
				return KappaEnumTypes::GenMatchingCode::IS_ELE_FROM_TAU;
			}
			else if (pdgId == 13 && closest.isDirectPromptTauDecayProduct())
			{
				LOG(DEBUG) << "GenMatchingCode: IS_MUON_FROM_TAU";
				return KappaEnumTypes::GenMatchingCode::IS_MUON_FROM_TAU;
			}
		}
		LOG(DEBUG) << "GenMatchingCode: IS_FAKE , smallest dR = " << closestDR << " > 0.2";
		return KappaEnumTypes::GenMatchingCode::IS_FAKE;
	}
	return KappaEnumTypes::GenMatchingCode::NONE;
}

std::vector<RMFLV> GeneratorInfo::BuildGenTausUW(
		KappaTypes::event_type const& event
)
{
	std::vector<RMFLV> genTaus;

	if(event.m_genParticles && event.m_genParticles->size() > 0)
	{
		for (typename std::vector<KGenParticle>::iterator genParticle = event.m_genParticles->begin();
			genParticle != event.m_genParticles->end(); ++genParticle)
		{
			if (std::abs(genParticle->pdgId) == 15)
			{
				if (genParticle->isPrompt())
				{
					if (genParticle->nDaughters() > 0)
					{
						bool has_tau_daughter = false;
						bool has_lepton_daughter = false;
						for (unsigned dau = 0; dau < genParticle->nDaughters(); ++dau)
						{
							assert(genParticle->daughterIndex(dau) >= 0);
							int pdgId = std::abs(event.m_genParticles->at(genParticle->daughterIndex(dau)).pdgId);
							if (pdgId == 15) has_tau_daughter = true;
							if (pdgId == 11 || pdgId == 13) has_lepton_daughter = true;
						}
						if (has_tau_daughter) continue;
						if (has_lepton_daughter) continue;

						RMFLV genTau;
						for (size_t dau = 0; dau < genParticle->nDaughters(); ++dau)
						{
							assert(genParticle->daughterIndex(dau) >= 0);
							int pdgId = std::abs(event.m_genParticles->at(genParticle->daughterIndex(dau)).pdgId);
							LOG(DEBUG) << "\tdaughter " << dau << " pdgId: " << event.m_genParticles->at(genParticle->daughterIndex(dau)).pdgId;
							if (pdgId == 12 || pdgId == 14 || pdgId == 16) continue;
							genTau += event.m_genParticles->at(genParticle->daughterIndex(dau)).p4;
						}
						genTaus.push_back(genTau);
					}
				}
			}
		}
	}
	return genTaus;
}
