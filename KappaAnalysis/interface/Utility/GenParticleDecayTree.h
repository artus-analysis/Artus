#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaTools/interface/HLTTools.h"

#include "Artus/Utility/interface/DefaultValues.h"
/**
   \brief Extended class for genParticles in HiggsAnalysis
   This class implements additional quantities: m_charge, final states in the decay 
   subtree of considered particles. The final states can be devided into one, three and five prongs.
  
*/
class GenParticleDecayTree {
public:
	GenParticleDecayTree(KGenParticle* genParticle = nullptr);
	~GenParticleDecayTree();
	
	bool m_finalState = false;
	std::vector<GenParticleDecayTree*> m_finalStates;
	std::vector<GenParticleDecayTree*> m_finalStateOneProngs;
	std::vector<GenParticleDecayTree*> m_finalStateThreeProngs;
	std::vector<GenParticleDecayTree*> m_finalStateFiveProngs;
	
	KGenParticle* m_genParticle;
	// will have 0 entries, if there are no daughters
	std::vector<GenParticleDecayTree> m_daughters;

	enum class DecayMode : int
	{
		NONE = -1,
		E   = 1,
		M   = 2,
		//Greater 3 is hadronic
		PI = 4,
		KPLUS = 5,
		KSTAR = 6,
		RHO = 7,
		AONE   = 8,
		//These should appear for HiggsBoson
		TAU = 10,
		TAUTAU = 11
	};

	DecayMode m_decayMode = DecayMode::NONE;

	void CreateFinalStates(GenParticleDecayTree* root);
	void CreateFinalStateProngs(GenParticleDecayTree* root);
	void SetCharge();
	int GetCharge() const;
	void SetDetectable();
	bool IsDetectable() const;
	void DetermineDecayMode(GenParticleDecayTree* root);
	void SetDecayMode(GenParticleDecayTree* tauDaughter);
	
private:
	int m_charge = 5;
	bool m_detectable = false;
	std::vector<int> m_positiveChargedParticlePdgIds = 
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
	
	std::vector<int> m_negativeChargedParticlePdgIds =
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
	
	std::vector<int> m_neutralParticlePdgIds =
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
	
	std::vector<int> m_detectableParticlePdgIds = 
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
