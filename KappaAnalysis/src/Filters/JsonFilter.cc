#include "Artus/KappaAnalysis/interface/Filters/JsonFilter.h"

/** Filter events that are listed in a JSON file
 *
 *	The filter returns false in case the events is not selected
 *  in order to fully skip this event in the following analysis.
 *  This filter can savely be run as global or local filter.
 */

	std::string JsonFilter::GetFilterId() const {
		return "JsonFilter";
	}

	void JsonFilter::Init(KappaSettings const& settings)
	{
		FilterBase<KappaTypes>::Init(settings);
		
		m_runLumiSelector = RunLumiSelector(settings.GetJsonFiles(),
		                                    settings.GetPassRunLow(),
		                                    settings.GetPassRunHigh());
	}

	bool JsonFilter::DoesEventPass(KappaEvent const& event, KappaProduct const& product,
	                           KappaSettings const& settings) const
	{
		assert(event.m_eventInfo);
		
		return m_runLumiSelector.accept(event.m_eventInfo->nRun, (event.m_eventInfo->nLumi & 0x0000FFFF));
	}
