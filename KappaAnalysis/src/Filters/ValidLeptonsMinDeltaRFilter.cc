
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

//#include "HiggsAnalysis/KITHiggsToTauTau/interface/HttEnumTypes.h"
#include "Artus/KappaAnalysis/interface/Filters/ValidLeptonsMinDeltaRFilter.h"

/** Filter events with too close valid electrons
 */
ValidElectronsMinDeltaRFilter::ValidElectronsMinDeltaRFilter() :
	ValidLeptonsMinDeltaRFilterBase<KElectron>(&product_type::m_validElectrons,
		&setting_type::GetMinDeltaRValidElectrons)
{
}


/** Filter events with too close valid muons
 */
ValidMuonsMinDeltaRFilter::ValidMuonsMinDeltaRFilter() :
	ValidLeptonsMinDeltaRFilterBase<KMuon>(&product_type::m_validMuons,
		&setting_type::GetMinDeltaRValidMuons)
{
}



/** Filter events with too close valid taus
 */
ValidTausMinDeltaRFilter::ValidTausMinDeltaRFilter() :
	ValidLeptonsMinDeltaRFilterBase<KTau>(&product_type::m_validTaus,
		&setting_type::GetMinDeltaRValidTaus)
{
}


/** Filter events with too close valid leptons
 */
ValidLeptonsMinDeltaRFilter::ValidLeptonsMinDeltaRFilter() :
	ValidLeptonsMinDeltaRFilterBase<KLepton>(&product_type::m_validLeptons,
		&setting_type::GetMinDeltaRValidLeptons)
{
}
