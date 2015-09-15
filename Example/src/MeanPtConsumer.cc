/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#include "Artus/Example/interface/MeanPtConsumer.h"

void MeanPtConsumer::ProcessFilteredEvent(TraxEvent const& event,
		TraxProduct const& product, TraxSettings const& setting)
{
	// i++; m = (m * (i-1) + pt) / i = m + (pt - m) / i
	m_itemCount++;
	m_mean += (static_cast<double>(event.m_floatPtSim) - m_mean) / static_cast<double>(m_itemCount);
}
