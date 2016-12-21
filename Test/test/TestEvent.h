
#pragma once

#include "Artus/Core/interface/EventBase.h"

class TestEvent : public EventBase {
public:
	TestEvent() : iVal(0) {}

	int iVal;

};
