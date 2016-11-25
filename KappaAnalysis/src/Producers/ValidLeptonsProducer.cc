
#include "Artus/KappaAnalysis/interface/Producers/ValidLeptonsProducer.h"


void ValidLeptonsProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
{
	// start with empty vectors
	product.m_validLeptons.clear();
	product.m_invalidLeptons.clear();
	
	// copy vectors
	product.m_validLeptons.insert(product.m_validLeptons.end(),
	                              product.m_validElectrons.begin(), product.m_validElectrons.end());
	
	product.m_validLeptons.insert(product.m_validLeptons.end(),
	                              product.m_validMuons.begin(), product.m_validMuons.end());
	
	product.m_validLeptons.insert(product.m_validLeptons.end(),
	                              product.m_validTaus.begin(), product.m_validTaus.end());
	
	product.m_invalidLeptons.insert(product.m_invalidLeptons.end(),
	                              product.m_invalidElectrons.begin(), product.m_invalidElectrons.end());
	
	product.m_invalidLeptons.insert(product.m_invalidLeptons.end(),
	                              product.m_invalidMuons.begin(), product.m_invalidMuons.end());
	
	product.m_invalidLeptons.insert(product.m_invalidLeptons.end(),
	                              product.m_invalidTaus.begin(), product.m_invalidTaus.end());

	// sort vectors of leptons by pt
	std::sort(product.m_validLeptons.begin(), product.m_validLeptons.end(),
	          [](KLepton const* lepton1, KLepton const* lepton2) -> bool
	          { return lepton1->p4.Pt() > lepton2->p4.Pt(); });
	std::sort(product.m_invalidLeptons.begin(), product.m_invalidLeptons.end(),
	          [](KLepton const* lepton1, KLepton const* lepton2) -> bool
	          { return lepton1->p4.Pt() > lepton2->p4.Pt(); });
}

