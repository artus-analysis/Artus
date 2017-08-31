#include "Artus/KappaAnalysis/interface/Filters/TriggerMatchingFilters.h"


/** Filter trigger matched electrons
 *  Required config tags:
 *  - MinNMatchedElectrons (default 0)
 */
std::string ElectronTriggerMatchingFilter::GetFilterId() const{
	return "ElectronTriggerMatchingFilter";
}

ElectronTriggerMatchingFilter::ElectronTriggerMatchingFilter() :
	TriggerMatchingFilterBase<KElectron>(&product_type::m_triggerMatchedElectrons,
	                                                 &product_type::m_validElectrons,
	                                                 &setting_type::GetMinNMatchedElectrons)
{
}


/** Filter trigger matched muons
 *  Required config tags:
 *  - MinNMatchedMuons (default 0)
 */
std::string MuonTriggerMatchingFilter::GetFilterId() const{
	return "MuonTriggerMatchingFilter";
}

MuonTriggerMatchingFilter::MuonTriggerMatchingFilter() :
	TriggerMatchingFilterBase<KMuon>(&product_type::m_triggerMatchedMuons,
	                                             &product_type::m_validMuons,
	                                             &setting_type::GetMinNMatchedMuons)
{
}




/** Filter trigger matched taus
 *  Required config tags:
 *  - MinNMatchedTaus (default 0)
 */
std::string TauTriggerMatchingFilter::GetFilterId() const{
	return "TauTriggerMatchingFilter";
}

TauTriggerMatchingFilter::TauTriggerMatchingFilter() :
	TriggerMatchingFilterBase<KTau>(&product_type::m_triggerMatchedTaus,
	                                              &product_type::m_validTaus,
	                                              &setting_type::GetMinNMatchedTaus)
{
}




/** Filter trigger matched jets
 *  Required config tags:
 *  - MinNMatchedJets (default 0)
 */
std::string JetTriggerMatchingFilter::GetFilterId() const{
	return "JetTriggerMatchingFilter";
}

JetTriggerMatchingFilter::JetTriggerMatchingFilter() :
	TriggerMatchingFilterBase<KBasicJet>(&product_type::m_triggerMatchedJets,
	                                              &product_type::m_validJets,
	                                              &setting_type::GetMinNMatchedJets)
{
}
