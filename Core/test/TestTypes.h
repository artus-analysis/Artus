#pragma once


#include "TestData.h"
#include "TestMetaData.h"
#include "TestSettings.h"

struct TestTypes {
	typedef TestData event_type;
	typedef TestMetaData meta_type;
	typedef TestSettings setting_type;
};
