#include "Artus/KappaAnalysis/interface/Filters/TriggerMatchingFilters.h"


/** Filter trigger matched electrons
 *  Required config tags:
 *  - MinNMatchedElectrons (default 0)
 */
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
JetTriggerMatchingFilter::JetTriggerMatchingFilter() :
	TriggerMatchingFilterBase<KBasicJet>(&KappaProduct::m_triggerMatchedJets,
	                                              &KappaProduct::m_validJets,
	                                              &KappaSettings::GetMinNMatchedJets)
{
}
