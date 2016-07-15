#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

#include "Artus/Utility/interface/Utility.h"

#include <boost/regex.hpp>

/**
   \brief SampleStitchingWeightProducer
   Config tags:
   - Fill me with something meaningful

*/

class SampleStitchingWeightProducer : public KappaProducerBase {
public:

	std::string GetProducerId() const override;

	virtual void Init(KappaSettings const& settings);

	void Produce( KappaEvent const& event,
			KappaProduct & product,
			KappaSettings const& settings) const override;

private:

	std::map<size_t, std::vector<double> > stitchingWeightsByIndex;
	std::map<size_t, std::vector<double> > stitchingWeightsHighMassByIndex;
	std::map<std::string, std::vector<double> > stitchingWeightsByName;
	std::map<std::string, std::vector<double> > stitchingWeightsHighMassByName;

};
