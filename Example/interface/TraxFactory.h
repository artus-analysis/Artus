/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/FactoryBase.h"


#include "TraxTypes.h"

#include "TraxPipelineSettings.h"
#include "TraxEvent.h"
#include "TraxProduct.h"

// producer
#include "PtCorrectionProducer.h"
#include "PtCorrectionProducerLocal.h"

// filter
#include "PtFilter.h"

// consumer
#include "MeanPtConsumer.h"
#include "TraxNtupleConsumer.h"

class TraxFactory: public FactoryBase<TraxTypes> {
public:

	TraxFactory() : FactoryBase<TraxTypes>() {
	}

	virtual ~TraxFactory() {}

	virtual TraxGlobalProducerBase * createGlobalProducer ( std::string const& id )
		ARTUS_CPP11_OVERRIDE
	{
		if ( PtCorrectionProducer().GetProducerId() == id )
			return new PtCorrectionProducer();
		else
			return FactoryBase<TraxTypes>::createGlobalProducer( id );
	}

	virtual TraxLocalProducerBase * createLocalProducer ( std::string const& id )
		ARTUS_CPP11_OVERRIDE
	{
		if ( PtCorrectionProducerLocal().GetProducerId() == id )
			return new PtCorrectionProducerLocal();
		else
			return FactoryBase<TraxTypes>::createLocalProducer( id );
	}

	virtual TraxConsumerBase * createConsumer ( std::string const& id )
		ARTUS_CPP11_OVERRIDE
	{
		if ( MeanPtConsumer().GetConsumerId() == id )
			return new MeanPtConsumer();
		else if ( TraxNtupleConsumer().GetConsumerId() == id )
			return new TraxNtupleConsumer();
		else
			return FactoryBase<TraxTypes>::createConsumer( id );
	}

	virtual TraxFilterBase * createFilter ( std::string const& id )
		ARTUS_CPP11_OVERRIDE
	{
		if ( PtFilter().GetFilterId() == id )
			return new PtFilter();
		else
			return FactoryBase<TraxTypes>::createFilter( id );
	}


};
