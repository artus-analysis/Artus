/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/PipelineRunner.h"

#include "TestEvent.h"
#include "TestProduct.h"
#include "TestSettings.h"

struct TestTypes {
	typedef TestEvent event_type;
	typedef TestProduct product_type;
	typedef TestSettings setting_type;
	//typedef TestGlobalSettings global_setting_type;
};

