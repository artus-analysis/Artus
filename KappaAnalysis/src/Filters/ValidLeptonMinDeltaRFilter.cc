
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

//#include "HiggsAnalysis/KITHiggsToTauTau/interface/HttEnumTypes.h"
#include "Artus/KappaAnalysis/interface/Filters/ValidLeptonMinDeltaRFilter.h"

/** Filter events with too close valid electrons
 */
std::string ValidElectronMinDeltaRFilter::GetFilterId() const{
	return "ValidElectronMinDeltaRFilter";
}

ValidElectronMinDeltaRFilter::ValidElectronMinDeltaRFilter() :
	ValidLeptonMinDeltaRFilterBase<KElectron>(&product_type::m_validElectrons,
		&setting_type::GetMinDeltaRValidElectrons)
{
}


/** Filter events with too close valid muons
 */
std::string ValidMuonMinDeltaRFilter::GetFilterId() const{
	return "ValidMuonMinDeltaRFilter";
}

ValidMuonMinDeltaRFilter::ValidMuonMinDeltaRFilter() :
	ValidLeptonMinDeltaRFilterBase<KMuon>(&product_type::m_validMuons,
		&setting_type::GetMinDeltaRValidMuons)
{
}



/** Filter events with too close valid taus
 */
std::string ValidTauMinDeltaRFilter::GetFilterId() const{
	return "ValidTauMinDeltaRFilter";
}

ValidTauMinDeltaRFilter::ValidTauMinDeltaRFilter() :
	ValidLeptonMinDeltaRFilterBase<KTau>(&product_type::m_validTaus,
		&setting_type::GetMinDeltaRValidTaus)
{
}


/** Filter events with too close valid leptons
 */
std::string ValidLeptonMinDeltaRFilter::GetFilterId() const{
	return "ValidLeptonMinDeltaRFilter";
}

ValidLeptonMinDeltaRFilter::ValidLeptonMinDeltaRFilter() :
	ValidLeptonMinDeltaRFilterBase<KLepton>(&product_type::m_validLeptons,
		&setting_type::GetMinDeltaRValidLeptons)
{
}
