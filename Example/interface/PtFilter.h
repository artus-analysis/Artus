/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "TraxTypes.h"

class PtFilter: public TraxFilterBase
{
public:

	virtual ~PtFilter();

	std::string GetFilterId() const override;

	void Init(TraxSettings const& globalSettings) override;

	bool DoesEventPass(TraxEvent const& event,
			TraxProduct const& product, TraxSettings const& settings) const override;
};

