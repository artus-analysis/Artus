
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "KappaTools/RootTools/RunLumiReader.h"

#include "Artus/Core/interface/GlobalProducerBase.h"


/** Filter events that are listed in a JSON file
 *
 *	The Filter returns false in case the events is not selected
 *  in order to fully skip this event in the following analysis
 */
template<class TTypes>
class JsonFilter: public GlobalProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;

	virtual std::string GetProducerId() {
		return "json_filter";
	}
	
	JsonFilter(const std::vector<std::string>& jsonFiles=std::vector<std::string>(), const run_id passRunLow=1, const run_id passRunHigh=0) :
		GlobalProducerBase<TTypes>(),
		m_runLumiSelector(jsonFiles, passRunLow, passRunHigh)
	{
		
	}

	virtual bool ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const
	{
		return m_runLumiSelector.accept(event.m_eventMetadata->nRun, event.m_eventMetadata->nLumi);
	}

private:
	RunLumiSelector m_runLumiSelector;
};
