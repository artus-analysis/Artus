
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

//#include "HiggsAnalysis/KITHiggsToTauTau/interface/HttEnumTypes.h"
#include "Artus/KappaAnalysis/interface/Filters/ValidLeptonsMinDeltaRFilter.h"

/** Filter events with too close valid electrons
 */
std::string ValidElectronsMinDeltaRFilter::GetFilterId() const{
	return "ValidElectronsMinDeltaRFilter";
}

ValidElectronsMinDeltaRFilter::ValidElectronsMinDeltaRFilter() :
	ValidLeptonsMinDeltaRFilterBase<KElectron>(&product_type::m_validElectrons,
		&setting_type::GetMinDeltaRValidElectrons)
{
}


/** Filter events with too close valid muons
 */
std::string ValidMuonsMinDeltaRFilter::GetFilterId() const{
	return "ValidMuonsMinDeltaRFilter";
}

ValidMuonsMinDeltaRFilter::ValidMuonsMinDeltaRFilter() :
	ValidLeptonsMinDeltaRFilterBase<KMuon>(&product_type::m_validMuons,
		&setting_type::GetMinDeltaRValidMuons)
{
}



/** Filter events with too close valid taus
 */
std::string ValidTausMinDeltaRFilter::GetFilterId() const{
	return "ValidTausMinDeltaRFilter";
}

ValidTausMinDeltaRFilter::ValidTausMinDeltaRFilter() :
	ValidLeptonsMinDeltaRFilterBase<KTau>(&product_type::m_validTaus,
		&setting_type::GetMinDeltaRValidTaus)
{
}


/** Filter events with too close valid leptons
 */
std::string ValidLeptonsMinDeltaRFilter::GetFilterId() const{
	return "ValidLeptonsMinDeltaRFilter";
}

ValidLeptonsMinDeltaRFilter::ValidLeptonsMinDeltaRFilter() :
	ValidLeptonsMinDeltaRFilterBase<KLepton>(&product_type::m_validLeptons,
		&setting_type::GetMinDeltaRValidLeptons)
{
}
