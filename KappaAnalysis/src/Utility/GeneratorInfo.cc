
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
