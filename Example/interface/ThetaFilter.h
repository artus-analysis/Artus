/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "TraxTypes.h"

class ThetaFilter: public TraxFilterBase {
public:

	virtual ~ThetaFilter() {
	}

	std::string GetFilterId() const override {
		return "filter_theta";
	}

	bool DoesEventPass(TraxEvent const& event,
			TraxProduct const& product,
			TraxSettings const& globalSettings) const override
	{
		const float lowCut = globalSettings.GetFilterThetaLow();
		const float highCut = globalSettings.GetFilterThetaHigh();

		assert(lowCut <= highCut);

		return ((lowCut <= event.m_floatTheSim) && (highCut > event.m_floatTheSim));
	}

};


