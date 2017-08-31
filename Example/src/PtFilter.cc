
#include "Artus/Example/interface/PtFilter.h"


PtFilter::~PtFilter()
{
}

std::string PtFilter::GetFilterId() const
{
	return "filter_pt";
}

void PtFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<TraxTypes>::Init(settings, metadata);
}

bool PtFilter::DoesEventPass(event_type const& event, product_type const& product,
		setting_type const& settings, metadata_type const& metadata) const
{

	const float lowCut = settings.GetFilterPtLow();
	const float highCut = settings.GetFilterPtHigh();

	assert(lowCut <= highCut);

	return ((lowCut <= event.m_floatPtSim) && (highCut > event.m_floatPtSim));
}

