
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Artus/KappaAnalysis/interface/Filters/BeamScrapingFilter.h"


std::string BeamScrapingFilter::GetFilterId() const {
	return "BeamScrapingFilter";
}

void BeamScrapingFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);

	m_purityRatio = settings.GetMinPurityRatio();
}

bool BeamScrapingFilter::DoesEventPass(event_type const& event, product_type const& product,
                                       setting_type const& settings, metadata_type const& metadata) const
{
	assert(event.m_trackSummary);

	return (static_cast<double>(event.m_trackSummary->nTracksHQ) /
	        static_cast<double>(event.m_trackSummary->nTracks) > m_purityRatio);
}
