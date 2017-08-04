
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
		KGenParticle closest = event.m_genParticles->at(0);
		double closestDR = 999;
		for (typename std::vector<KGenParticle>::iterator genParticle = event.m_genParticles->begin();
			genParticle != event.m_genParticles->end(); ++genParticle)
		{
			double tmpDR = ROOT::Math::VectorUtil::DeltaR(lepton->p4, genParticle->p4);
			if (tmpDR < closestDR)
			{
				closest = *genParticle;
				closestDR = tmpDR;
			}
		}
		int pdgId = std::abs(closest.pdgId);
		if (pdgId == 11 && closest.p4.Pt() > 8. && closest.isPrompt())
			return KappaEnumTypes::GenMatchingCode::IS_ELE_PROMPT;
		else if (pdgId == 13 && closest.p4.Pt() > 8. && closest.isPrompt())
			return KappaEnumTypes::GenMatchingCode::IS_MUON_PROMPT;
		else if (pdgId == 11 && closest.p4.Pt() > 8. && closest.isDirectPromptTauDecayProduct())
			return KappaEnumTypes::GenMatchingCode::IS_ELE_FROM_TAU;
		else if (pdgId == 13 && closest.p4.Pt() > 8. && closest.isDirectPromptTauDecayProduct())
			return KappaEnumTypes::GenMatchingCode::IS_MUON_FROM_TAU;
		else
		{
			std::vector<RMFLV> genTaus = BuildGenTausUW(event);
			for(auto genTau : genTaus)
			{
				if (ROOT::Math::VectorUtil::DeltaR(lepton->p4, genTau) < 0.2)
					return KappaEnumTypes::GenMatchingCode::IS_TAU_HAD_DECAY;
			}
			return KappaEnumTypes::GenMatchingCode::IS_FAKE;
		}
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
