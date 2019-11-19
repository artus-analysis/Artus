
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Artus/KappaAnalysis/interface/Filters/GoodPrimaryVertexFilter.h"


std::string GoodPrimaryVertexFilter::GetFilterId() const {
	return "GoodPrimaryVertexFilter";
}

void GoodPrimaryVertexFilter::Init(setting_type const& settings, metadata_type& metadata)
{
	FilterBase<KappaTypes>::Init(settings, metadata);
	m_maxPrimaryVertexZ = settings.GetMaxPrimaryVertexZ();
	m_maxPrimaryVertexRho = settings.GetMaxPrimaryVertexRho();
	m_minPrimaryVertexFitnDOF = settings.GetMinPrimaryVertexFitnDOF();
}

bool GoodPrimaryVertexFilter::DoesEventPass(event_type const& event, product_type const& product,
                                            setting_type const& settings, metadata_type const& metadata) const
{
	assert(event.m_vertexSummary);
	return (!(event.m_vertexSummary->pv.fake()) &&
	   (std::abs(event.m_vertexSummary->pv.position.Z()) < m_maxPrimaryVertexZ) &&
	   (std::abs(event.m_vertexSummary->pv.position.Rho()) < m_maxPrimaryVertexRho) &&
	   (event.m_vertexSummary->pv.nDOF >= m_minPrimaryVertexFitnDOF));
}
