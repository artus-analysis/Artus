#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "Artus/Core/interface/FilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"


class HCALNoiseFilter: public FilterBase<KappaTypes>
{
public:
	std::string GetFilterId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
	bool DoesEventPass(event_type const& event, product_type const& product,
	                   setting_type const& settings, metadata_type const& metadata) const override;

private:
	bool useTS4TS5;
	float minRatio;
	float maxRatio;
	int minHPDHits;
	int minRBXHits;
	int minHPDNoOtherHits;
	int minZeros;
	float minHighEHitTime;
	float maxHighEHitTime;
	float maxRBXEMF;
	int minNumIsolatedNoiseChannels;
	float minIsolatedNoiseSumE;
	float minIsolatedNoiseSumEt;
};
