
#include "Artus/KappaAnalysis/interface/Utility/GenParticleDecayTree.h"
#include "Artus/Utility/interface//Utility.h"


GenParticleDecayTree::GenParticleDecayTree(KGenParticle* genParticle) :
	m_genParticle(genParticle)
{
}

GenParticleDecayTree::~GenParticleDecayTree() {
}

void GenParticleDecayTree::CreateFinalStates(GenParticleDecayTree* root)
{
	if (m_finalState)
	{
		root->m_finalStates.push_back(this);
	}
	
	for (std::vector<GenParticleDecayTree>::iterator daughter = m_daughters.begin();
	     daughter != m_daughters.end(); ++daughter)
	{
		daughter->CreateFinalStates(root);
	}
}

void GenParticleDecayTree::CreateFinalStateProngs(GenParticleDecayTree* root)
{
	int chargedParticles = 0;
	CreateFinalStates(root);
	for (std::vector<GenParticleDecayTree*>::iterator finalState = m_finalStates.begin();
	     finalState != m_finalStates.end(); ++finalState)
	{
		if (std::abs((*finalState)->GetCharge()) == 1)
		{
			++chargedParticles;
		}
	}
	if (chargedParticles == 1)
	{
		m_finalStateOneProngs = m_finalStates;
	}
	else if (chargedParticles == 3)
	{
		m_finalStateThreeProngs = m_finalStates;
	}
	else if (chargedParticles == 5)
	{
		m_finalStateFiveProngs = m_finalStates;
	}
}

void GenParticleDecayTree::SetCharge()
{
	if (Utility::Contains(m_positiveChargedParticlePdgIds, m_genParticle->pdgId))
	{
		m_charge = 1;
	}
	else if (Utility::Contains(m_negativeChargedParticlePdgIds, m_genParticle->pdgId))
	{
		m_charge = -1;
	}
	else if (Utility::Contains(m_neutralParticlePdgIds, m_genParticle->pdgId))
	{
		m_charge = 0;
	}
}

int GenParticleDecayTree::GetCharge() const
{
	return m_charge;
}

void GenParticleDecayTree::SetDetectable()
{
	m_detectable = Utility::Contains(m_detectableParticlePdgIds, m_genParticle->pdgId);
}

bool GenParticleDecayTree::IsDetectable() const
{
	return m_detectable;
}

void GenParticleDecayTree::DetermineDecayMode(GenParticleDecayTree* root)
{
	for (std::vector<GenParticleDecayTree>::iterator daughter = root->m_daughters.begin();
	     daughter != root->m_daughters.end(); ++daughter)
	{
		SetDecayMode(&(*daughter));
		if(m_decayMode == DecayMode::NONE)
		{
			DetermineDecayMode(&(*daughter));
		}
	}
}

void GenParticleDecayTree::SetDecayMode(GenParticleDecayTree* tauDaughter)
{
	int pdgId = std::abs(tauDaughter->m_genParticle->pdgId);
	if (pdgId == DefaultValues::pdgIdTau)
	{
		m_decayMode = DecayMode::TAU;
	}
	else if (pdgId == DefaultValues::pdgIdPiPlus)
	{
		m_decayMode = DecayMode::PI;
	}
	else if (pdgId == DefaultValues::pdgIdKPlus)
	{
		m_decayMode = DecayMode::KPLUS;
	}
	else if (pdgId == DefaultValues::pdgIdKStar)
	{
		m_decayMode = DecayMode::KSTAR;
	}
	else if (pdgId == DefaultValues::pdgIdRhoPlus770)
	{
		m_decayMode = DecayMode::RHO;
	}
	else if (pdgId == DefaultValues::pdgIdAOnePlus1260)
	{
		m_decayMode = DecayMode::AONE;
	}
	else if (pdgId == DefaultValues::pdgIdMuon)
	{
		m_decayMode = DecayMode::M;
	}
	else if (pdgId == DefaultValues::pdgIdElectron)
	{
		m_decayMode = DecayMode::E;
	}
}

