
#pragma once

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
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

	void Init(setting_type const& settings, metadata_type& metadata) override;

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override;


private:
		std::vector<double> m_pileupWeights;
		double m_bins;

};

