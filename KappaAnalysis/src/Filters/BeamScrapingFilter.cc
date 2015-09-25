
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Artus/KappaAnalysis/interface/Filters/BeamScrapingFilter.h"


std::string BeamScrapingFilter::GetFilterId() const {
	return "BeamScrapingFilter";
}

void BeamScrapingFilter::Init(setting_type const& settings)
{
	FilterBase<KappaTypes>::Init(settings);

	m_purityRatio = settings.GetMinPurityRatio();
}

bool BeamScrapingFilter::DoesEventPass(KappaEvent const& event, KappaProduct const& product,
                                       KappaSettings const& settings) const
{
	assert(event.m_trackSummary);

	return (static_cast<double>(event.m_trackSummary->nTracksHQ) /
	        static_cast<double>(event.m_trackSummary->nTracks) > m_purityRatio);
}

