/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"

#include "TraxTypes.h"

class PtFilter: public TraxFilterBase {
public:

	virtual ~PtFilter() {
	}

	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "filter_pt";
	}
	
	virtual void InitGlobal(global_setting_type const& globalSettings)  ARTUS_CPP11_OVERRIDE
	{
		FilterBase<TraxTypes>::InitGlobal(globalSettings);
	}
	
	virtual void InitLocal(setting_type const& settings)  ARTUS_CPP11_OVERRIDE
	{
		FilterBase<TraxTypes>::InitLocal(settings);
	}

	virtual bool DoesEventPassLocal(TraxEvent const& event,
			TraxProduct const& product,
            TraxPipelineSettings const& settings ) const
		ARTUS_CPP11_OVERRIDE
	{

		const float lowCut = settings.GetFilterPtLow();
		const float highCut = settings.GetFilterPtHigh();

		assert(lowCut <= highCut);

		return ((lowCut <= event.m_floatPtSim) && (highCut > event.m_floatPtSim));
	}
};


