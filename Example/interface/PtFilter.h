/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"

#include "TraxTypes.h"

class PtFilter: public TraxFilterBase
{
public:

	virtual ~PtFilter()
	{
	}

	virtual std::string GetFilterId() const override
	{
		return "filter_pt";
	}

	virtual void Init(TraxSettings const& globalSettings) override
	{
		FilterBase<TraxTypes>::Init(globalSettings);
	}

	virtual bool DoesEventPass(TraxEvent const& event,
			TraxProduct const& product, TraxSettings const& settings) const override
	{

		const float lowCut = settings.GetFilterPtLow();
		const float highCut = settings.GetFilterPtHigh();

		assert(lowCut <= highCut);

		return ((lowCut <= event.m_floatPtSim) && (highCut > event.m_floatPtSim));
	}
};

