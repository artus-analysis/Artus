#include "Artus/KappaAnalysis/interface/Producers/MatchedLeptonsProducer.h"


std::string MatchedLeptonsProducer::GetProducerId() const {
	return "MatchedLeptonsProducer";
}

void MatchedLeptonsProducer::Produce(event_type const& event, product_type& product,
                                     setting_type const& settings, metadata_type const& metadata) const
{
	// start with empty vectors
	product.m_genParticleMatchedLeptons.clear();

	product.m_genParticleMatchedLeptons.insert(product.m_genParticleMatchedElectrons.begin(), product.m_genParticleMatchedElectrons.end());

	product.m_genParticleMatchedLeptons.insert(product.m_genParticleMatchedMuons.begin(), product.m_genParticleMatchedMuons.end());

	product.m_genParticleMatchedLeptons.insert(product.m_genParticleMatchedTaus.begin(), product.m_genParticleMatchedTaus.end());


	//Maybe create inverse map 
}
