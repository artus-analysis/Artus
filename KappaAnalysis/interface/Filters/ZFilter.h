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
	ZFilter();

	bool DoesEventPass(KappaEvent const& event,
	                   KappaProduct const& product,
	                   KappaSettings const& settings) const override;
};
