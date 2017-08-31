#include "Artus/KappaAnalysis/interface/Filters/nPUFilter.h"

std::string nPUFilter::GetFilterId() const {
	return "nPUFilter";
}

bool nPUFilter::DoesEventPass(event_type const& event, product_type const& product,
                              setting_type const& settings, metadata_type const& metadata) const 
{
	assert(event.m_eventInfo);
	if (static_cast<KGenEventInfo*>(event.m_eventInfo)->nPU >= settings.GetMinNPU() && static_cast<KGenEventInfo*>(event.m_eventInfo)->nPU <= settings.GetMaxNPU())
	{
		return true;
	}
	else
	{
		return false;
	}
}
