/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "TraxTypes.h"

class PtCorrectionProducerLocal: public TraxProducerBase {
public:

	std::string GetProducerId() const override;

	void Init(setting_type const& settings) override;

	void Produce(TraxEvent const& event,
			TraxProduct & product,
			TraxSettings const& localSettings) const override;

};
