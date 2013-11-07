/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "TestData.h"
#include "TestMetaData.h"
#include "TestSettings.h"

struct TestTypes {
	typedef TestData event_type;
	typedef TestGlobalMetaData global_meta_type;
	typedef TestLocalMetaData local_meta_type;
	typedef TestSettings setting_type;
};
