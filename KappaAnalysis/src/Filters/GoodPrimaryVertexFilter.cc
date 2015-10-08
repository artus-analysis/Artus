
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Artus/KappaAnalysis/interface/Filters/GoodPrimaryVertexFilter.h"


std::string GoodPrimaryVertexFilter::GetFilterId() const {
	return "GoodPrimaryVertexFilter";
}

void GoodPrimaryVertexFilter::Init(setting_type const& settings)
{
	FilterBase<KappaTypes>::Init(settings);
	m_maxPrimaryVertexZ = settings.GetMaxPrimaryVertexZ();
	m_maxPrimaryVertexRho = settings.GetMaxPrimaryVertexRho();
	m_minPrimaryVertexFitnDOF = settings.GetMinPrimaryVertexFitnDOF();
}

bool GoodPrimaryVertexFilter::DoesEventPass(KappaEvent const& event, KappaProduct const& product,
                                               KappaSettings const& settings) const 
{
	assert(event.m_vertexSummary);
	return (!(event.m_vertexSummary->pv.fake()) &&
	   (std::abs(event.m_vertexSummary->pv.position.Z()) < m_maxPrimaryVertexZ) &&
	   (std::abs(event.m_vertexSummary->pv.position.Rho()) < m_maxPrimaryVertexRho) &&
	   (event.m_vertexSummary->pv.nDOF >= m_minPrimaryVertexFitnDOF));
}

