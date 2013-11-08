/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/GlobalProducerBase.h"

#include "TestTypes.h"

typedef GlobalProducerBase<TestTypes> TestProducerBase;

class TestGlobalProducer: public TestProducerBase {
public:
	virtual bool PopulateGlobalProduct(TestData const& data,
			TestGlobalProduct & product,
			TestGlobalSettings const& globalSettings) const ARTUS_CPP11_OVERRIDE {
		product.iGlobalProduct = data.iVal + 5 + globalSettings.GetOffset();
		return true;
	}
};
