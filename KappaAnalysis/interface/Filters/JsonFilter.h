
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "KappaTools/RootTools/RunLumiReader.h"

#include "Artus/Core/interface/FilterBase.h"


/** Filter events that are listed in a JSON file
 *
 *	The filter returns false in case the events is not selected
 *  in order to fully skip this event in the following analysis.
 *  This filter can savely be run as global or local filter.
 */
template<class TTypes>
class JsonFilter: public FilterBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "JsonFilter";
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE
	{
		FilterBase<TTypes>::Init(settings);
		
		m_runLumiSelector = RunLumiSelector(settings.GetJsonFiles(),
		                                    settings.GetPassRunLow(),
		                                    settings.GetPassRunHigh());
	}

	virtual bool DoesEventPass(event_type const& event, product_type const& product,
	                           setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		return m_runLumiSelector.accept(event.m_eventMetadata->nRun, event.m_eventMetadata->nLumi);
	}


private:
	RunLumiSelector m_runLumiSelector;
};
