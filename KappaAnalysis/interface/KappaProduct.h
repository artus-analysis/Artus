
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "KappaTools/RootTools/HLTTools.h"


class KappaProduct {
public:
	KappaProduct() {};
	~KappaProduct() {};

	// Electrons
	std::vector<KDataElectron*> m_validElectrons;
	std::vector<KDataElectron*> m_invalidElectrons;

	// Muons
	std::vector<KDataMuon*> m_validMuons;
	std::vector<KDataMuon*> m_invalidMuons;

	// Taus
	std::vector<KDataPFTau*> m_validTaus;
	std::vector<KDataPFTau*> m_invalidTaus;

	// Jets
	std::vector<KDataPFJet*> m_validJets;
	std::vector<KDataPFJet*> m_invalidJets;
	
	HLTTools* m_hltInfo = 0;
	std::string selectedHltName = "";

private:
	
};
