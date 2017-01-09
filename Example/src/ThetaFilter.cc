
#include "Artus/Example/interface/ThetaFilter.h"


ThetaFilter::~ThetaFilter() {
}

std::string ThetaFilter::GetFilterId() const {
	return "filter_theta";
}

bool ThetaFilter::DoesEventPass(TraxEvent const& event,
		TraxProduct const& product,
		TraxSettings const& globalSettings) const
{
	const float lowCut = globalSettings.GetFilterThetaLow();
	const float highCut = globalSettings.GetFilterThetaHigh();

	assert(lowCut <= highCut);

	return ((lowCut <= event.m_floatTheSim) && (highCut > event.m_floatTheSim));
}

