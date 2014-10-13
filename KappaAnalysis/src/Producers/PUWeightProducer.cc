
#include "TH1.h"

#include "Artus/KappaAnalysis/interface/Producers/PUWeightProducer.h"


std::string PUWeightProducer::GetProducerId() const {
	return "PUWeightProducer";
}

void PUWeightProducer::Init(KappaSettings const& settings) {
	KappaProducerBase::Init(settings);
	
	const std::string histogramName = "pileup";
	LOG(DEBUG) << "\tLoading pile-up weights from files...";
	LOG(DEBUG) << "\t\t" << settings.GetPileupWeightFile() << "/" << histogramName;
	TFile file(settings.GetPileupWeightFile().c_str(), "READONLY");
	TH1D* pileupHistogram = (TH1D*) file.Get(histogramName.c_str());

	m_pileupWeights.clear();
	for (int i = 1; i <= pileupHistogram->GetNbinsX(); ++i)
	{
		m_pileupWeights.push_back(pileupHistogram->GetBinContent(i));
	}
	m_bins = 1.0 / pileupHistogram->GetBinWidth(1);
	delete pileupHistogram;
	file.Close();
}

void PUWeightProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
{
	assert(event.m_genEventMetadata != NULL);
	
	double npu = event.m_genEventMetadata->numPUInteractionsTruth;
	if (npu < m_pileupWeights.size())
		product.m_weights["puWeight"] = m_pileupWeights.at(int(npu * m_bins));
	else
		product.m_weights["puWeight"] = 0.0;
}

