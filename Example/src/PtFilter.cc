
#include "Artus/Example/interface/PtFilter.h"


PtFilter::~PtFilter()
{
}

std::string PtFilter::GetFilterId() const
{
	return "filter_pt";
}

void PtFilter::Init(TraxSettings const& globalSettings)
{
	FilterBase<TraxTypes>::Init(globalSettings);
}

bool PtFilter::DoesEventPass(TraxEvent const& event,
		TraxProduct const& product, TraxSettings const& settings) const
{

	const float lowCut = settings.GetFilterPtLow();
	const float highCut = settings.GetFilterPtHigh();

	assert(lowCut <= highCut);

	return ((lowCut <= event.m_floatPtSim) && (highCut > event.m_floatPtSim));
}

