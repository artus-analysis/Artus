#include "Artus/KappaAnalysis/interface/Filters/TriggerMatchingFilters.h"


/** Filter trigger matched electrons
 *  Required config tags:
 *  - MinNMatchedElectrons (default 0)
 */
std::string ElectronTriggerMatchingFilter::GetFilterId() const{
	return "ElectronTriggerMatchingFilter";
}

ElectronTriggerMatchingFilter::ElectronTriggerMatchingFilter() :
	TriggerMatchingFilterBase<KElectron>(&KappaProduct::m_triggerMatchedElectrons,
	                                                 &KappaProduct::m_validElectrons,
	                                                 &KappaSettings::GetMinNMatchedElectrons)
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
	TriggerMatchingFilterBase<KMuon>(&KappaProduct::m_triggerMatchedMuons,
	                                             &KappaProduct::m_validMuons,
	                                             &KappaSettings::GetMinNMatchedMuons)
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
	TriggerMatchingFilterBase<KTau>(&KappaProduct::m_triggerMatchedTaus,
	                                              &KappaProduct::m_validTaus,
	                                              &KappaSettings::GetMinNMatchedTaus)
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
	TriggerMatchingFilterBase<KBasicJet>(&KappaProduct::m_triggerMatchedJets,
	                                              &KappaProduct::m_validJets,
	                                              &KappaSettings::GetMinNMatchedJets)
{
}
