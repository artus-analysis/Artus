
#include "Artus/KappaAnalysis/interface/Producers/MuonCorrectionsProducer.h"


std::string MuonCorrectionsProducer::GetProducerId() const {
	return "MuonCorrectionsProducer";
}

void MuonCorrectionsProducer::Init(setting_type const& settings) 
{
	KappaProducerBase::Init(settings);
}

void MuonCorrectionsProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
{
	assert(event.m_muons);

	// create a copy of all muons in the event
	product.m_correctedMuons.clear();
	product.m_correctedMuons.resize(event.m_muons->size());
	size_t muonIndex = 0;
	for (KMuons::const_iterator muon = event.m_muons->begin();
		 muon != event.m_muons->end(); ++muon)
	{
		product.m_correctedMuons[muonIndex] = std::shared_ptr<KMuon>(new KMuon(*muon));
		++muonIndex;
	}
	
	// perform corrections on copied muons
	for (std::vector<std::shared_ptr<KMuon> >::iterator muon = product.m_correctedMuons.begin();
		 muon != product.m_correctedMuons.end(); ++muon)
	{
		// No general correction available
	
		// perform possible analysis-specific corrections
		AdditionalCorrections(muon->get(), event, product, settings);
	}
	
	// sort vectors of corrected muons by pt
	std::sort(product.m_correctedMuons.begin(), product.m_correctedMuons.end(),
	          [](std::shared_ptr<KMuon> muon1, std::shared_ptr<KMuon> muon2) -> bool
	          { return muon1->p4.Pt() > muon2->p4.Pt(); });
}


// Can be overwritten for analysis-specific use cases
void MuonCorrectionsProducer::AdditionalCorrections(KMuon* muon, KappaEvent const& event,
                                   KappaProduct& product, KappaSettings const& settings) const
{
}

