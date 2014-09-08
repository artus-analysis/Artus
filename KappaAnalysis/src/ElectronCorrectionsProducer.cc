
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
	// create a copy of all electrons in the event
	product.m_correctedElectrons.clear();
	product.m_correctedElectrons.resize(event.m_electrons->size());
	size_t electronIndex = 0;
	for (KDataElectrons::const_iterator electron = event.m_electrons->begin();
		 electron != event.m_electrons->end(); ++electron)
	{
		product.m_correctedElectrons[electronIndex] = std::shared_ptr<KDataElectron>(new KDataElectron(*electron));
		++electronIndex;
	}
	
	// perform corrections on copied electrons
	for (std::vector<std::shared_ptr<KDataElectron> >::iterator electron = product.m_correctedElectrons.begin();
		 electron != product.m_correctedElectrons.end(); ++electron)
	{
		// No general correction available
	
		// perform possible analysis-specific corrections
		AdditionalCorrections(electron->get(), event, product, settings);
	}
	
	// sort vectors of corrected electrons by pt
	std::sort(product.m_correctedElectrons.begin(), product.m_correctedElectrons.end(),
	          [](std::shared_ptr<KDataElectron> electron1, std::shared_ptr<KDataElectron> electron2) -> bool
	          { return electron1->p4.Pt() > electron2->p4.Pt(); });
}


