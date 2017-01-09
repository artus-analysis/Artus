
#pragma once

#include "Artus/Core/interface/PipelineRunner.h"
#include "Artus/Core/interface/EventProviderBase.h"

class TestEventProvider: public EventProviderBase<TestTypes> {
public:
	~TestEventProvider() {}

	TestTypes::event_type const& GetCurrentEvent() const override {
		return m_event;
	}
	bool GetEntry(long long lEventNumber) override {
		return lEventNumber < GetEntries();
	}
	long long GetEntries() const override {
		return 10;
	}

	TestTypes::event_type m_event;
};
