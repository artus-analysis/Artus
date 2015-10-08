#include "Artus/KappaAnalysis/interface/Filters/nPUFilter.h"

std::string nPUFilter::GetFilterId() const {
	return "nPUFilter";
}

bool nPUFilter::DoesEventPass(KappaEvent const& event, KappaProduct const& product,
                                       KappaSettings const& settings) const 
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
