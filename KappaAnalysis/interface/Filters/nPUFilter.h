
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "KappaTools/RootTools/RunLumiReader.h"

#include "Artus/Core/interface/FilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/** Filter events by white/black lists for run/lumi/event numbers
 *  Config tags:
 *   - RunWhitelist
 *   - RunBlacklist
 *   - LumiWhitelist
 *   - LumiBlacklist
 *   - EventWhitelist
 *   - EventBlacklist
 *   - MatchRunLumiEventTuples (optional)
 */
class nPUFilter: public FilterBase<KappaTypes>
{

public:


	std::string GetFilterId() const override;

	bool DoesEventPass(KappaEvent const& event, KappaProduct const& product,
	                           KappaSettings const& settings) const override;


private:
	
	//bool MatchWhiteBlackLists(uint64_t item, std::vector<uint64_t> const& whitelist, std::vector<uint64_t> const& blacklist) const;
};
