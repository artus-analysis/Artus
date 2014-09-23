
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

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;

	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE;

	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE;


private:
		std::vector<double> m_pileupWeights;
		double m_bins;

};

