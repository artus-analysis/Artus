#include "Artus/KappaAnalysis/interface/Producers/MatchedLeptonsProducer.h"


std::string MatchedLeptonsProducer::GetProducerId() const {
	return "MatchedLeptonsProducer";
}

void MatchedLeptonsProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
{
	// start with empty vectors
	product.m_genMatchedLeptons.clear();

	product.m_genMatchedLeptons.insert(product.m_genMatchedElectrons.begin(), product.m_genMatchedElectrons.end());

	product.m_genMatchedLeptons.insert(product.m_genMatchedMuons.begin(), product.m_genMatchedMuons.end());

	product.m_genMatchedLeptons.insert(product.m_genMatchedTaus.begin(), product.m_genMatchedTaus.end());


	//Maybe create inverse map 
}
