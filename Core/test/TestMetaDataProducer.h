#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/MetaDataProducerBase.h"

#include "TestTypes.h"

typedef GlobalMetaDataProducerBase<TestTypes> TestMetaDataProducerBase;

class TestGlobalMetaDataProducer: public TestMetaDataProducerBase {
public:
	virtual bool PopulateGlobalMetaData(TestData const& data,
			TestMetaData & metaData, TestSettings const& globalSettings) const ARTUS_CPP11_OVERRIDE {
		metaData.iGlobalMetaData = data.iVal + 5;
		return true;
	}
};
