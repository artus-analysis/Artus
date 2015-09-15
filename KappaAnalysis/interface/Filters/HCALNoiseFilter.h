#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "Artus/Core/interface/FilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"


class HCALNoiseFilter: public FilterBase<KappaTypes>
{
public:
	virtual std::string GetFilterId() const override;
	virtual void Init(setting_type const& settings) override;
	virtual bool DoesEventPass(KappaEvent const& event, KappaProduct const& product,
	                           KappaSettings const& settings) const override;

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
