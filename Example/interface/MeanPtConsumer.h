/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "TraxTypes.h"

/*
 * IMPORTANT NOTE:
 * Not nummericaly stable. Don't use in real world applications
 * This is just to illustrate an easy consumer.
 */

class MeanPtConsumer: public TraxConsumerBase {
public:
	MeanPtConsumer();

	std::string GetConsumerId() const override;

	void ProcessFilteredEvent(TraxEvent const& event,
			TraxProduct const& product,
			setting_type const& setting) override;

	void Finish(setting_type const& setting ) override;

private:
	long long m_itemCount;
	double m_mean;
};

