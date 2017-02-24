
#include "Artus/KappaAnalysis/interface/Filters/GenMatchingFilters.h"


ElectronGenMatchingFilter::ElectronGenMatchingFilter() :
	GenMatchingFilterBase<KElectron>(&KappaProduct::m_genParticleMatchedElectrons,
	                                 &KappaProduct::m_validElectrons)
{
}


MuonGenMatchingFilter::MuonGenMatchingFilter() :
	GenMatchingFilterBase<KMuon>(&KappaProduct::m_genParticleMatchedMuons,
	                             &KappaProduct::m_validMuons)
{
}


TauGenMatchingFilter::TauGenMatchingFilter() :
	GenMatchingFilterBase<KTau>(&KappaProduct::m_genParticleMatchedTaus,
	                            &KappaProduct::m_validTaus)
{
}


JetGenMatchingFilter::JetGenMatchingFilter() :
	GenMatchingFilterBase<KBasicJet>(&KappaProduct::m_genParticleMatchedJets,
	                                 &KappaProduct::m_validJets)
{
}

