
#pragma once

#include "Artus/Core/interface/PipelineRunner.h"

#include "TestEvent.h"
#include "TestProduct.h"
#include "TestSettings.h"
#include "TestMetadata.h"

struct TestTypes {
	typedef TestEvent event_type;
	typedef TestProduct product_type;
	typedef TestSettings setting_type;
	typedef TestMetadata metadata_type;
};

