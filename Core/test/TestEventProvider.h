/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/PipelineRunner.h"
#include "Artus/Core/interface/EventProviderBase.h"

class TestEventProvider: public EventProviderBase<TestEvent> {
	virtual TestEvent const& GetCurrentEvent() const ARTUS_CPP11_OVERRIDE {
		return m_event;
	}
	virtual bool GotoEvent(long long lEventNumber) ARTUS_CPP11_OVERRIDE {
		return true;
	}
	virtual long long GetOverallEventCount() const ARTUS_CPP11_OVERRIDE {
		return 10;
	}

	TestEvent m_event;
};
