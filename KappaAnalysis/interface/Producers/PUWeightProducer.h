
#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/**
   \brief GlobalProducer, for PU weight.
   
   This producer calculates a weight for a event based on the number of pileup truth
   interaction in a MC skim (m_genEventMetadata->numPUInteractionsTruth) and the
   weights from an output file of the weightCalc.py script.
   
   This producer needs the following config tags:
    - PileupWeightFile
*/

class PUWeightProducer: public KappaProducerBase
{

public:

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "PUWeightProducer";
	}

	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE {
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

	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE
	{
		assert(event.m_genEventMetadata != NULL);
		double npu = event.m_genEventMetadata->numPUInteractionsTruth;
		if (npu < m_pileupWeights.size())
			product.m_weights["puWeight"] = m_pileupWeights.at(int(npu * m_bins));
		else
			product.m_weights["puWeight"] = 0.0;
	}


private:
		std::vector<double> m_pileupWeights;
		double m_bins;

};

