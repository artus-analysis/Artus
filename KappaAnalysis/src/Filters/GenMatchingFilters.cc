
#include "Artus/KappaAnalysis/interface/Filters/GenMatchingFilters.h"


std::string ElectronGenMatchingFilter::GetFilterId() const
{
	return "ElectronGenMatchingFilter";
}

ElectronGenMatchingFilter::ElectronGenMatchingFilter() :
	GenMatchingFilterBase<KElectron>(&product_type::m_genParticleMatchedElectrons,
	                                 &product_type::m_validElectrons)
{
}


std::string MuonGenMatchingFilter::GetFilterId() const
{
	return "MuonGenMatchingFilter";
}

MuonGenMatchingFilter::MuonGenMatchingFilter() :
	GenMatchingFilterBase<KMuon>(&product_type::m_genParticleMatchedMuons,
	                             &product_type::m_validMuons)
{
}


std::string TauGenMatchingFilter::GetFilterId() const
{
	return "TauGenMatchingFilter";
}

TauGenMatchingFilter::TauGenMatchingFilter() :
	GenMatchingFilterBase<KTau>(&product_type::m_genParticleMatchedTaus,
	                            &product_type::m_validTaus)
{
}


std::string JetGenMatchingFilter::GetFilterId() const
{
	return "JetGenMatchingFilter";
}

JetGenMatchingFilter::JetGenMatchingFilter() :
	GenMatchingFilterBase<KBasicJet>(&product_type::m_genParticleMatchedJets,
	                                 &product_type::m_validJets)
{
}

