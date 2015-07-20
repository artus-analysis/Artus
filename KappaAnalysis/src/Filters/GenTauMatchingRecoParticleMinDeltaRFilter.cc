
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Artus/KappaAnalysis/interface/Filters/GenTauMatchingRecoParticleMinDeltaRFilter.h"

/** Filter events with too close gen matched electrons
 */
std::string GenTauMatchingRecoElectronMinDeltaRFilter::GetFilterId() const{
	return "GenTauMatchingRecoElectronMinDeltaRFilter";
}

GenTauMatchingRecoElectronMinDeltaRFilter::GenTauMatchingRecoElectronMinDeltaRFilter() :
	GenTauMatchingRecoParticleMinDeltaRFilterBase<KElectron>(&product_type::m_genTauMatchedElectrons,
				&setting_type::GetMinDeltaRMatchedRecoElectrons)
{
}


/** Filter events with too close gen matched muons
 */
std::string GenTauMatchingRecoMuonMinDeltaRFilter::GetFilterId() const{
	return "GenTauMatchingRecoMuonMinDeltaRFilter";
}

GenTauMatchingRecoMuonMinDeltaRFilter::GenTauMatchingRecoMuonMinDeltaRFilter() :
	GenTauMatchingRecoParticleMinDeltaRFilterBase<KMuon>(&product_type::m_genTauMatchedMuons,
				&setting_type::GetMinDeltaRMatchedRecoMuons)
{
}



/** Filter events with too close gen matched taus
 */
std::string GenTauMatchingRecoTauMinDeltaRFilter::GetFilterId() const{
	return "GenTauMatchingRecoTauMinDeltaRFilter";
}

GenTauMatchingRecoTauMinDeltaRFilter::GenTauMatchingRecoTauMinDeltaRFilter() :
	GenTauMatchingRecoParticleMinDeltaRFilterBase<KTau>(&product_type::m_genTauMatchedTaus,
				&setting_type::GetMinDeltaRMatchedRecoTaus)
{
}
