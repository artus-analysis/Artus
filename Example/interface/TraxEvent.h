/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/KappaAnalysis/interface/KappaEvent.h"

struct TraxEvent : KappaEvent {

	float m_floatTheSim;
	float m_floatPtSim;
	float m_floatPSim;
	float m_floatPzSim;
};


