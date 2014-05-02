/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"

#include "TraxTypes.h"

class ThetaFilter: public TraxFilterBase {
public:

	virtual ~ThetaFilter() {
	}

	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "filter_theta";
	}
	
	virtual void InitGlobal(global_setting_type const& globalSettings)  ARTUS_CPP11_OVERRIDE
	{
		FilterBase<TraxTypes>::InitGlobal(globalSettings);
	}
	
	virtual void InitLocal(setting_type const& settings)  ARTUS_CPP11_OVERRIDE
	{
		FilterBase<TraxTypes>::InitLocal(settings);
	}

	virtual bool DoesEventPassGlobal(TraxEvent const& event,
			TraxProduct const& product,
			TraxGlobalSettings const& globalSettings) const
	ARTUS_CPP11_OVERRIDE
	{
		const float lowCut = globalSettings.GetFilterThetaLow();
		const float highCut = globalSettings.GetFilterThetaHigh();

		assert(lowCut <= highCut);

		return ((lowCut <= event.m_floatTheSim) && (highCut > event.m_floatTheSim));
	}

};


