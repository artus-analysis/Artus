
#include "TH1.h"

#include "Artus/KappaAnalysis/interface/Producers/PUWeightProducer.h"


std::string PUWeightProducer::GetProducerId() const {
	return "PUWeightProducer";
}

void PUWeightProducer::Init(setting_type const& settings, metadata_type& metadata) {
	KappaProducerBase::Init(settings, metadata);

	const std::string histogramName = "pileup";
	LOG(DEBUG) << "\tLoading pile-up weights from files...";
	LOG(DEBUG) << "\t\t" << settings.GetPileupWeightFile() << "/" << histogramName;
	TFile file(settings.GetPileupWeightFile().c_str(), "READONLY");
	TH1D* pileupHistogram = dynamic_cast<TH1D*>(file.Get(histogramName.c_str()));

	m_pileupWeights.clear();
	for (int i = 1; i <= pileupHistogram->GetNbinsX(); ++i)
	{
		m_pileupWeights.push_back(pileupHistogram->GetBinContent(i));
	}
	m_bins = 1.0 / pileupHistogram->GetBinWidth(1);
	delete pileupHistogram;
	file.Close();
}

void PUWeightProducer::Produce(event_type const& event, product_type& product,
                               setting_type const& settings, metadata_type const& metadata) const
{
	assert(event.m_genEventInfo != nullptr);

	unsigned int puBin = static_cast<unsigned int>(static_cast<double>(event.m_genEventInfo->nPUMean) * m_bins);
	if (puBin < m_pileupWeights.size())
		product.m_weights["puWeight"] = m_pileupWeights.at(puBin);
	else
		product.m_weights["puWeight"] = 1.0;
}

