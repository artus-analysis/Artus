
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "KappaTools/RootTools/RunLumiReader.h"

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

	virtual std::string GetFilterId() const override;

	virtual void Init(KappaSettings const& settings) override;
	virtual bool DoesEventPass(KappaEvent const& event, KappaProduct const& product,
	                           KappaSettings const& settings) const override;


private:
	RunLumiSelector m_runLumiSelector;
};
