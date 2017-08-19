
#include "Artus/Example/interface/ThetaFilter.h"


ThetaFilter::~ThetaFilter() {
}

std::string ThetaFilter::GetFilterId() const {
	return "filter_theta";
}

bool ThetaFilter::DoesEventPass(event_type const& event, product_type const& product,
		setting_type const& settings, metadata_type const& metadata) const
{
	const float lowCut = settings.GetFilterThetaLow();
	const float highCut = settings.GetFilterThetaHigh();

	assert(lowCut <= highCut);

	return ((lowCut <= event.m_floatTheSim) && (highCut > event.m_floatTheSim));
}

