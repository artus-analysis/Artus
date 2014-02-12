
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"


class KappaProduct {
public:
	KappaProduct() {};
	~KappaProduct() {};

	KDataMuons m_validMuons;
	KDataMuons m_invalidMuons;

	// use pointers to be able to handle tagged and untagged jets
	std::vector<KDataPFJet*> m_validJets;
	std::vector<KDataPFJet*> m_invalidJets;

private:
	
};
