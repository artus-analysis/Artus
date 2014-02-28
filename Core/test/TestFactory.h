/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/FactoryBase.h"

#include "TestTypes.h"

#include "TestGlobalProducer.h"
#include "TestLocalProducer.h"
#include "TestConsumer.h"
#include "Pipeline_t.h"



class TestFactory : public FactoryBase < TestTypes > {
public:

	virtual ~TestFactory() {}

	virtual TestGlobalProducer * createGlobalProducer ( std::string const& id )
		ARTUS_CPP11_OVERRIDE
	{

	}

	virtual TestLocalProducer * createLocalProducer ( std::string const& id )
		ARTUS_CPP11_OVERRIDE
	{

	}

	virtual TestConsumer * createConsumer ( std::string const& id )
		ARTUS_CPP11_OVERRIDE
	{

	}

	virtual TestFilter * createFilter ( std::string const& id )
		ARTUS_CPP11_OVERRIDE
	{

	}

};

