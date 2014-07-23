#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "KappaTools/RootTools/HLTTools.h"

#include "Artus/Utility/interface/DefaultValues.h"
/**
   \brief Extended class for genParticles in HiggsAnalysis
   This class implements additional quantities: charge, parent of the particle, and final states in the decay 
   subtree of considered particles. The final states can be devided into one, three and five prongs.
  
*/
class MotherDaughterBundle {
public:
	MotherDaughterBundle( KGenParticle* newnode ) : node( newnode ) {};
	~MotherDaughterBundle() {};
	// pointer to the parent
	MotherDaughterBundle * parent = 0;
	bool finalState = false;
	std::vector<MotherDaughterBundle*> finalStates;
	std::vector<MotherDaughterBundle*> finalStateOneProngs;
	std::vector<MotherDaughterBundle*> finalStateThreeProngs;
	std::vector<MotherDaughterBundle*> finalStateFiveProngs;
	// must be != null;
	KGenParticle* node;
	// will have 0 entries, if there are no daughters
	std::vector<MotherDaughterBundle> Daughters;
	void createFinalStates(MotherDaughterBundle* root)
	{
		if (this->finalState == true) root->finalStates.push_back(this);
		else if (this->Daughters.size() != 0)
		{
			for(unsigned int i = 0; i<this->Daughters.size(); ++i)
			{
				this->Daughters[i].createFinalStates(root);
			}
		}
	}
	void createFinalStateProngs(MotherDaughterBundle* root)
	{
		int chargedParticles = 0;
		this->createFinalStates(root);
		for(unsigned int i = 0; i<this->finalStates.size(); ++i)
		{
			if (this->finalStates[i]->getCharge() == 1 || this->finalStates[i]->getCharge() == -1)
			{
				chargedParticles++;
			}
		}
		if(chargedParticles==1)	this->finalStateOneProngs = this->finalStates;
		else if(chargedParticles==3) this->finalStateThreeProngs = this->finalStates;
		else if(chargedParticles==5) this->finalStateFiveProngs = this->finalStates;
	}
	void setCharge()
	{
		for (unsigned int i=0; i<positiveChargedParticlePdgIds.size(); ++i)
		{
			if (this->node->pdgId()==positiveChargedParticlePdgIds[i]) this->charge = 1;
		}
		for (unsigned int i=0; i<negativeChargedParticlePdgIds.size(); ++i)
		{
			if (this->node->pdgId()==negativeChargedParticlePdgIds[i]) this->charge = -1;
		}
		for (unsigned int i=0; i<notChargedParticlePdgIds.size(); ++i)
		{
			if (this->node->pdgId()==notChargedParticlePdgIds[i]) this->charge = 0;
		}
	}
	int getCharge() const
	{
		return this->charge;
	}
	void setDetectable()
	{
		for (unsigned int i=0; i<detectableParticlePdgIds.size(); ++i)
		{
			if (this->node->pdgId()==detectableParticlePdgIds[i]) this->detectable = true;
		}
	}
	bool isDetectable() const
	{
		return this->detectable;
	}
private:
	int charge = 5;
	bool detectable = false;
	std::vector<int> positiveChargedParticlePdgIds = 
		{
			-DefaultValues::pdgIdElectron,
			-DefaultValues::pdgIdMuon,
			-DefaultValues::pdgIdTau, 
			DefaultValues::pdgIdW, 
			DefaultValues::pdgIdPiPlus,
			DefaultValues::pdgIdRhoPlus770, 
			DefaultValues::pdgIdKPlus, 
			DefaultValues::pdgIdKStar, 
			DefaultValues::pdgIdAOnePlus1260
		};
	std::vector<int> negativeChargedParticlePdgIds =
		{
			DefaultValues::pdgIdElectron,
			DefaultValues::pdgIdMuon,
			DefaultValues::pdgIdTau, 
			-DefaultValues::pdgIdW, 
			-DefaultValues::pdgIdPiPlus,
			-DefaultValues::pdgIdRhoPlus770,
			-DefaultValues::pdgIdKPlus, 
			-DefaultValues::pdgIdKStar, 
			-DefaultValues::pdgIdAOnePlus1260
		};
	std::vector<int> notChargedParticlePdgIds =
		{
			DefaultValues::pdgIdNuE,
			-DefaultValues::pdgIdNuE,
			DefaultValues::pdgIdNuMu,
			-DefaultValues::pdgIdNuMu,
			DefaultValues::pdgIdNuTau,
			-DefaultValues::pdgIdNuTau,
			DefaultValues::pdgIdGamma,
			DefaultValues::pdgIdPiZero, 
			DefaultValues::pdgIdKLong, 
			DefaultValues::pdgIdEta,
			DefaultValues::pdgIdKShort
		};
	std::vector<int> detectableParticlePdgIds = 
		{
			DefaultValues::pdgIdGamma,
			DefaultValues::pdgIdPiPlus,
			-DefaultValues::pdgIdPiPlus,
			DefaultValues::pdgIdElectron,
			-DefaultValues::pdgIdElectron,
			DefaultValues::pdgIdMuon,
			-DefaultValues::pdgIdMuon,
			DefaultValues::pdgIdTau,
			-DefaultValues::pdgIdTau
		};
};
