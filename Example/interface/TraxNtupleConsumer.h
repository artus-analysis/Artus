/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Consumer/interface/NtupleConsumerBase.h"
#include "Artus/Utility/interface/DefaultValues.h"

#include "TraxTypes.h"

class TraxNtupleConsumer: public NtupleConsumerBase<TraxTypes> {

public:
	std::string GetConsumerId() const override;

private:
	float returnvalue(std::string string, TraxEvent const& event, TraxProduct const& product) override;

};
