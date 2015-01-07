
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Artus/KappaAnalysis/interface/Filters/HCALNoiseFilter.h"


std::string HCALNoiseFilter::GetFilterId() const {
	return "HCALNoiseFilter";
}

void HCALNoiseFilter::Init(setting_type const& settings)
{
	FilterBase<KappaTypes>::Init(settings);

	minRatio = -999.0;
	maxRatio = 999.0;
	minHPDHits = 17;
	minRBXHits = 999;
	minHPDNoOtherHits = 10;
	minZeros = 10;
	minHighEHitTime = -9999.0;
	maxHighEHitTime = 9999.0;
	maxRBXEMF = -999.0;
	minNumIsolatedNoiseChannels = 10;
	minIsolatedNoiseSumE = 50.0;
	minIsolatedNoiseSumEt = 25.0;
	useTS4TS5 = true;
}

bool HCALNoiseFilter::DoesEventPass(KappaEvent const& event, KappaProduct const& product,
                                               KappaSettings const& settings) const 
{
	assert(event.m_hcalNoiseSummary);
	bool pass;
	pass = (event.m_hcalNoiseSummary->minE2Over10TS >= minRatio) &&
	       (event.m_hcalNoiseSummary->maxE2Over10TS <= maxRatio) &&
	       (event.m_hcalNoiseSummary->maxHPDHits < minHPDHits) &&
	       (event.m_hcalNoiseSummary->maxRBXHits < minRBXHits) &&
	       (event.m_hcalNoiseSummary->maxHPDNoOtherHits < minHPDNoOtherHits) &&
	       (event.m_hcalNoiseSummary->maxZeros < minZeros) &&
	       (event.m_hcalNoiseSummary->min25GeVHitTime >= minHighEHitTime) &&
	       (event.m_hcalNoiseSummary->max25GeVHitTime <= maxHighEHitTime) &&
	       (event.m_hcalNoiseSummary->minRBXEMF >= maxRBXEMF) &&
	       (event.m_hcalNoiseSummary->numIsolatedNoiseChannels < minNumIsolatedNoiseChannels) &&
	       (event.m_hcalNoiseSummary->isolatedNoiseSumE < minIsolatedNoiseSumE) &&
	       (event.m_hcalNoiseSummary->isolatedNoiseSumEt < minIsolatedNoiseSumEt) &&
	       ((event.m_hcalNoiseSummary->hasBadRBXTS4TS5 == false) || (useTS4TS5 == false));
	return pass;
}

