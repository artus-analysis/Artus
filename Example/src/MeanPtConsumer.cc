/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#include "Artus/Example/interface/MeanPtConsumer.h"

void MeanPtConsumer::ProcessFilteredEvent(TraxEvent const& event,
		TraxProduct const& product, TraxSettings const& setting)
{

	m_mean = ((m_mean * m_itemCount) + event.m_floatPtSim) / (m_itemCount + 1);

	m_itemCount++;

}
