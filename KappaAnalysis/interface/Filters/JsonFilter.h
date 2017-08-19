
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "KappaTools/RootTools/interface/RunLumiReader.h"

#include "Artus/Core/interface/FilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/** Filter events that are listed in a JSON file
 *
 *	The filter returns false in case the events is not selected
 *  in order to fully skip this event in the following analysis.
 *  This filter can savely be run as global or local filter.
 */
class JsonFilter: public FilterBase<KappaTypes>
{

public:

	std::string GetFilterId() const override;

	void Init(setting_type const& settings, metadata_type& metadata) override;
	bool DoesEventPass(event_type const& event, product_type const& product,
	                   setting_type const& settings, metadata_type const& metadata) const override;


private:
	RunLumiSelector m_runLumiSelector;
};
