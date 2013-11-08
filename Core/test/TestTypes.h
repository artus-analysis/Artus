/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "TestEvent.h"
#include "TestProduct.h"
#include "TestSettings.h"

struct TestTypes {
	typedef TestEvent event_type;
	typedef TestGlobalProduct global_product_type;
	typedef TestLocalProduct local_product_type;
	typedef TestSettings setting_type;
	typedef TestGlobalSettings global_setting_type;
};
