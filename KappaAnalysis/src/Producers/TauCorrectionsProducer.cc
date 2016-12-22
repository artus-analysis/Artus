
#include "Artus/KappaAnalysis/interface/Producers/TauCorrectionsProducer.h"


std::string TauCorrectionsProducer::GetProducerId() const {
	return "TauCorrectionsProducer";
}

void TauCorrectionsProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);
}

void TauCorrectionsProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
{
	assert(event.m_taus);
	
	// create a copy of all taus in the event
	product.m_correctedTaus.clear();
	product.m_correctedTaus.resize(event.m_taus->size());
	size_t tauIndex = 0;
	for (KTaus::const_iterator tau = event.m_taus->begin();
		 tau != event.m_taus->end(); ++tau)
	{
		product.m_correctedTaus[tauIndex] = std::shared_ptr<KTau>(new KTau(*tau));
		product.m_originalLeptons[product.m_correctedTaus[tauIndex].get()] = &(*tau);
		++tauIndex;
	}
	
	// perform corrections on copied taus
	for (std::vector<std::shared_ptr<KTau> >::iterator tau = product.m_correctedTaus.begin();
		 tau != product.m_correctedTaus.end(); ++tau)
	{
		// No general correction available
	
		// perform possible analysis-specific corrections
		AdditionalCorrections(tau->get(), event, product, settings);

		// make sure to also save the corrected lepton and the matched genParticle in the map
		// if we match genParticles to all leptons
		if (settings.GetRecoTauMatchingGenParticleMatchAllTaus())
			product.m_genParticleMatchedTaus[tau->get()] =  &(*product.m_genParticleMatchedTaus[static_cast<KTau*>(const_cast<KLepton*>(product.m_originalLeptons[tau->get()]))]);
	}
	
	// sort vectors of corrected taus by pt
	std::sort(product.m_correctedTaus.begin(), product.m_correctedTaus.end(),
	          [](std::shared_ptr<KTau> tau1, std::shared_ptr<KTau> tau2) -> bool
	          { return tau1->p4.Pt() > tau2->p4.Pt(); });
}

void TauCorrectionsProducer::AdditionalCorrections(KTau* tau, KappaEvent const& event,
	                                   KappaProduct& product, KappaSettings const& settings) const
{
}

