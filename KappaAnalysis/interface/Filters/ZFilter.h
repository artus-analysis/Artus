#pragma once

#include "Artus/KappaAnalysis/interface/KappaTypes.h"

#include "Artus/Core/interface/FilterBase.h"

/** Filter class for Z boson.
 *
 *  Needs to run after the Z producer.
 */

class ZFilter : public FilterBase<KappaTypes>
{
  public:
	std::string GetFilterId() const override;

	ZFilter();

	bool DoesEventPass(event_type const& event, product_type const& product,
	                   setting_type const& settings, metadata_type const& metadata) const override;
};
