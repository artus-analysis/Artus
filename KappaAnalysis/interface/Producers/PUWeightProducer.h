
#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/**
   \brief GlobalProducer, for PU weight.
   
   This producer calculates a weight for a event based on the number of pileup truth
   interaction in a MC skim (m_genEventInfo->nPUMean) and the
   weights from an output file of the weightCalc.py script.
   
   This producer needs the following config tags:
    - PileupWeightFile
*/

class PUWeightProducer: public KappaProducerBase
{

public:

	std::string GetProducerId() const override;

	void Init(KappaSettings const& settings) override;

	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;


private:
		std::vector<double> m_pileupWeights;
		double m_bins;

};

