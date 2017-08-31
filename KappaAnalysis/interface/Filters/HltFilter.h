
#pragma once

#include <memory>

#include "KappaTools/RootTools/interface/RunLumiReader.h"

#include "Artus/Core/interface/FilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/** Filter events for that a previously selected HLT trigger has fired.
 *
 *  The selected HLT trigger has to be defined in the product by the HltProducer.
 *  Therefore this filter cannot meaningfully run as a global pre-filter
 *  which gets an empty product.
 */
class HltFilter: public FilterBase<KappaTypes> {

public:

	std::string GetFilterId() const override;
	bool DoesEventPass(event_type const& event, product_type const& product,
	                   setting_type const& settings, metadata_type const& metadata) const override;

};

