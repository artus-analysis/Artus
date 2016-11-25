
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Artus/KappaAnalysis/interface/Filters/GenTauMatchingRecoParticleMinDeltaRFilter.h"

/** Filter events with too close gen matched electrons
 */
GenTauMatchingRecoElectronMinDeltaRFilter::GenTauMatchingRecoElectronMinDeltaRFilter() :
	GenTauMatchingRecoParticleMinDeltaRFilterBase<KElectron>(&product_type::m_genTauMatchedElectrons,
				&setting_type::GetMinDeltaRMatchedRecoElectrons)
{
}


/** Filter events with too close gen matched muons
 */
GenTauMatchingRecoMuonMinDeltaRFilter::GenTauMatchingRecoMuonMinDeltaRFilter() :
	GenTauMatchingRecoParticleMinDeltaRFilterBase<KMuon>(&product_type::m_genTauMatchedMuons,
				&setting_type::GetMinDeltaRMatchedRecoMuons)
{
}



/** Filter events with too close gen matched taus
 */
GenTauMatchingRecoTauMinDeltaRFilter::GenTauMatchingRecoTauMinDeltaRFilter() :
	GenTauMatchingRecoParticleMinDeltaRFilterBase<KTau>(&product_type::m_genTauMatchedTaus,
				&setting_type::GetMinDeltaRMatchedRecoTaus)
{
}
