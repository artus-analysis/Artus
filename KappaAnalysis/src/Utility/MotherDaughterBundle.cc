
#include "Artus/KappaAnalysis/interface/Utility/MotherDaughterBundle.h"

MotherDaughterBundle::MotherDaughterBundle(KGenParticle* newNode) :
	m_node(newNode)
{
}

MotherDaughterBundle::~MotherDaughterBundle() {
}

void MotherDaughterBundle::CreateFinalStates(MotherDaughterBundle* root)
{
	if (this->m_finalState)
		root->m_finalStates.push_back(this);
	else if (this->m_daughters.size() != 0)
	{
		for(unsigned int i = 0; i<this->m_daughters.size(); ++i)
		{
			this->m_daughters[i].CreateFinalStates(root);
		}
	}
}

void MotherDaughterBundle::CreateFinalStateProngs(MotherDaughterBundle* root)
{
	int chargedParticles = 0;
	this->CreateFinalStates(root);
	for(unsigned int i = 0; i<this->m_finalStates.size(); ++i)
	{
		if (this->m_finalStates[i]->GetCharge() == 1 || this->m_finalStates[i]->GetCharge() == -1)
		{
			++chargedParticles;
		}
	}
	if(chargedParticles==1)	this->m_finalStateOneProngs = this->m_finalStates;
	else if(chargedParticles==3) this->m_finalStateThreeProngs = this->m_finalStates;
	else if(chargedParticles==5) this->m_finalStateFiveProngs = this->m_finalStates;
}

void MotherDaughterBundle::SetCharge()
{
	for (unsigned int i=0; i<m_positiveChargedParticlePdgIds.size(); ++i)
	{
		if (this->m_node->pdgId==m_positiveChargedParticlePdgIds[i]) this->m_charge = 1;
	}
	for (unsigned int i=0; i<m_negativeChargedParticlePdgIds.size(); ++i)
	{
		if (this->m_node->pdgId==m_negativeChargedParticlePdgIds[i]) this->m_charge = -1;
	}
	for (unsigned int i=0; i<m_nonChargedParticlePdgIds.size(); ++i)
	{
		if (this->m_node->pdgId==m_nonChargedParticlePdgIds[i]) this->m_charge = 0;
	}
}

int MotherDaughterBundle::GetCharge() const
{
	return this->m_charge;
}

void MotherDaughterBundle::SetDetectable()
{
	for (unsigned int i=0; i<m_detectableParticlePdgIds.size(); ++i)
	{
		if (this->m_node->pdgId==m_detectableParticlePdgIds[i]) this->m_detectable = true;
	}
}

bool MotherDaughterBundle::IsDetectable() const
{
	return this->m_detectable;
}

void MotherDaughterBundle::DetermineDecayMode(MotherDaughterBundle* root)
{
	for (auto tauDaughter = root->m_daughters.begin(); tauDaughter != root->m_daughters.end();++tauDaughter)
	{
		this->SetDecayMode(&(*tauDaughter));
		if(this->m_decayMode == DecayMode::NONE)
		{
			this->DetermineDecayMode(&(*tauDaughter));
		}
	}
}

void MotherDaughterBundle::SetDecayMode(MotherDaughterBundle* tauDaughters)
{
	int pdgId = std::abs(tauDaughters->m_node->pdgId);
	if (pdgId == DefaultValues::pdgIdTau)
	{
		if (this->m_decayMode == DecayMode::TAU) this->m_decayMode = DecayMode::TAUTAU;
		this->m_decayMode = DecayMode::TAU;
	}
	else if (pdgId == DefaultValues::pdgIdPiPlus) this->m_decayMode = DecayMode::PI;
	else if (pdgId == DefaultValues::pdgIdKPlus) this->m_decayMode = DecayMode::KPLUS;
	else if (pdgId == DefaultValues::pdgIdKStar) this->m_decayMode = DecayMode::KSTAR;
	else if (pdgId == DefaultValues::pdgIdRhoPlus770) this->m_decayMode = DecayMode::RHO;
	else if (pdgId == DefaultValues::pdgIdAOnePlus1260) this->m_decayMode = DecayMode::AONE;
	else if (pdgId == DefaultValues::pdgIdMuon) this->m_decayMode = DecayMode::M;
	else if (pdgId == DefaultValues::pdgIdElectron) this->m_decayMode = DecayMode::E;
}

