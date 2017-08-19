
#include "Artus/KappaAnalysisExample/interface/DiMuonMassFilter.h"


	
std::string DiMuonMassFilter::GetFilterId() const
{
	return "DiMuonMassFilter";
}

bool DiMuonMassFilter::DoesEventPass(event_type const& event, product_type const& product,
                                     setting_type const& settings, metadata_type const& metadata) const
{
	return (product.m_diMuonSystem.M() > settings.GetDiMuonMassLowerCut());
}

