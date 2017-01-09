
#include "Artus/KappaAnalysis/interface/Filters/GenMatchingFilters.h"


std::string ElectronGenMatchingFilter::GetFilterId() const
{
	return "ElectronGenMatchingFilter";
}

ElectronGenMatchingFilter::ElectronGenMatchingFilter() :
	GenMatchingFilterBase<KElectron>(&KappaProduct::m_genParticleMatchedElectrons,
	                                 &KappaProduct::m_validElectrons)
{
}


std::string MuonGenMatchingFilter::GetFilterId() const
{
	return "MuonGenMatchingFilter";
}

MuonGenMatchingFilter::MuonGenMatchingFilter() :
	GenMatchingFilterBase<KMuon>(&KappaProduct::m_genParticleMatchedMuons,
	                             &KappaProduct::m_validMuons)
{
}


std::string TauGenMatchingFilter::GetFilterId() const
{
	return "TauGenMatchingFilter";
}

TauGenMatchingFilter::TauGenMatchingFilter() :
	GenMatchingFilterBase<KTau>(&KappaProduct::m_genParticleMatchedTaus,
	                            &KappaProduct::m_validTaus)
{
}


std::string JetGenMatchingFilter::GetFilterId() const
{
	return "JetGenMatchingFilter";
}

JetGenMatchingFilter::JetGenMatchingFilter() :
	GenMatchingFilterBase<KBasicJet>(&KappaProduct::m_genParticleMatchedJets,
	                                 &KappaProduct::m_validJets)
{
}

