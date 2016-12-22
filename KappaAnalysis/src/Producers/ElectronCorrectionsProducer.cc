
#include "Artus/KappaAnalysis/interface/Producers/ElectronCorrectionsProducer.h"

std::string ElectronCorrectionsProducer::GetProducerId() const {
	return "ElectronCorrectionsProducer";
}

void ElectronCorrectionsProducer::Init(setting_type const& settings)
{
	KappaProducerBase::Init(settings);
}

void ElectronCorrectionsProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                          KappaSettings const& settings) const
{
	assert(event.m_electrons);

	// create a copy of all electrons in the event
	product.m_correctedElectrons.clear();
	product.m_correctedElectrons.resize(event.m_electrons->size());
	size_t electronIndex = 0;
	for (KElectrons::const_iterator electron = event.m_electrons->begin();
		 electron != event.m_electrons->end(); ++electron)
	{
		product.m_correctedElectrons[electronIndex] = std::shared_ptr<KElectron>(new KElectron(*electron));
		product.m_originalLeptons[product.m_correctedElectrons[electronIndex].get()] = &(*electron);
		++electronIndex;
	}
	
	// perform corrections on copied electrons
	for (std::vector<std::shared_ptr<KElectron> >::iterator electron = product.m_correctedElectrons.begin();
		 electron != product.m_correctedElectrons.end(); ++electron)
	{
		// No general correction available
	
		// perform possible analysis-specific corrections
		AdditionalCorrections(electron->get(), event, product, settings);

		// make sure to also save the corrected lepton and the matched genParticle in the map
		// if we match genParticles to all leptons
		if (settings.GetRecoElectronMatchingGenParticleMatchAllElectrons())
			product.m_genParticleMatchedElectrons[electron->get()] =  &(*product.m_genParticleMatchedElectrons[static_cast<KElectron*>(const_cast<KLepton*>(product.m_originalLeptons[electron->get()]))]);
		if (settings.GetMatchAllElectronsGenTau())
		{
			product.m_genTauMatchedElectrons[electron->get()] = &(*product.m_genTauMatchedElectrons[static_cast<KElectron*>(const_cast<KLepton*>(product.m_originalLeptons[electron->get()]))]);
			product.m_genTauMatchedLeptons[electron->get()] = &(*product.m_genTauMatchedLeptons[const_cast<KLepton*>(product.m_originalLeptons[electron->get()])]);
		}
	}
	
	// sort vectors of corrected electrons by pt
	std::sort(product.m_correctedElectrons.begin(), product.m_correctedElectrons.end(),
	          [](std::shared_ptr<KElectron> electron1, std::shared_ptr<KElectron> electron2) -> bool
	          { return electron1->p4.Pt() > electron2->p4.Pt(); });
}

void ElectronCorrectionsProducer::AdditionalCorrections(KElectron* electron, KappaEvent const& event,
                                                        KappaProduct& product, KappaSettings const& settings) const
{
}

