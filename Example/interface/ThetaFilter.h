/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "TraxTypes.h"

class ThetaFilter: public TraxFilterBase {
public:

	virtual ~ThetaFilter();

	std::string GetFilterId() const override;

	bool DoesEventPass(TraxEvent const& event,
			TraxProduct const& product,
			TraxSettings const& globalSettings) const override;

};


