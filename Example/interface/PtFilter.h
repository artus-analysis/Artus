#pragma once

#include "Artus/Core/interface/Cpp11Support.h"

#include "TraxFilterBase.h"

class PtFilter: public TraxFilterBase {
public:

	virtual ~PtFilter() {
	}

	virtual std::string GetFilterId() {
		return "filter_pt";
	}

	virtual bool DoesEventPass(TraxEventData const& event,
			TraxGlobalMetaData const& metaData,
			TraxPipelineSettings const& settings) {

		const float lowCut = settings.GetFilterPtLow();
		const float highCut = settings.GetFilterPtHigh();

		assert(lowCut <= highCut);

		return ((lowCut <= event.m_floatPtSim) && (highCut > event.m_floatPtSim));
	}
};
