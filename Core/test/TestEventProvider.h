#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/EventPipelineRunner.h"

class TestEventProvider: public EventProvider<TestData> {
	virtual TestData const& GetCurrentEvent() const ARTUS_CPP11_OVERRIDE {
		return m_data;
	}
	virtual bool GotoEvent(long long lEventNumber) ARTUS_CPP11_OVERRIDE {
		return true;
	}
	virtual long long GetOverallEventCount() const ARTUS_CPP11_OVERRIDE {
		return 10;
	}

	TestData m_data;
};
