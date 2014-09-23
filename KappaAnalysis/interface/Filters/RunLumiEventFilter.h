
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
 */
class RunLumiEventFilter: public FilterBase<KappaTypes>
{

public:


	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE;

	virtual bool DoesEventPass(KappaEvent const& event, KappaProduct const& product,
	                           KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE;


private:
	
	bool MatchWhiteBlackLists(int item, std::vector<int> const& whitelist, std::vector<int> const& blacklist) const;
};
