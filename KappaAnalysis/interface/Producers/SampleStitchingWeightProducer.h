#pragma once

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
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

	virtual void Init(setting_type const& settings, metadata_type& metadata);

	void Produce(event_type const& event, product_type & product,
	             setting_type const& settings, metadata_type const& metadata) const override;

private:

	std::map<size_t, std::vector<double> > stitchingWeightsByIndex;
	std::map<size_t, std::vector<double> > stitchingWeightsHighMassByIndex;
	std::map<std::string, std::vector<double> > stitchingWeightsByName;
	std::map<std::string, std::vector<double> > stitchingWeightsHighMassByName;

};
