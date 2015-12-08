/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

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
#include "ThetaFilter.h"

// consumer
#include "MeanPtConsumer.h"
#include "TraxNtupleConsumer.h"
#include "CutFlowConsumer.h"

class TraxFactory: public FactoryBase/*<TraxTypes>*/ {
public:

	TraxFactory() : FactoryBase/*<TraxTypes>*/() {
	}

	virtual ~TraxFactory() {}

	ProducerBaseUntemplated* createProducer(std::string const& id) override
	{
		if ( PtCorrectionProducer().GetProducerId() == id )
			return new PtCorrectionProducer();
		else if ( PtCorrectionProducerLocal().GetProducerId() == id )
			return new PtCorrectionProducerLocal();
		else
			return FactoryBase/*<TraxTypes>*/::createProducer( id );
	}

	ConsumerBaseUntemplated* createConsumer(std::string const& id) override
	{
		/*if ( MeanPtConsumer().GetConsumerId() == id )
			return new MeanPtConsumer();
		else if ( TraxNtupleConsumer().GetConsumerId() == id )
			return new TraxNtupleConsumer();
		else if ( CutFlowConsumer().GetConsumerId() == id )
			return new CutFlowConsumer();
		else*/
			return FactoryBase/*<TraxTypes>*/::createConsumer( id );
	}

	FilterBaseUntemplated* createFilter(std::string const& id) override
	{
		if ( PtFilter().GetFilterId() == id )
			return new PtFilter();
		else if ( ThetaFilter().GetFilterId() == id )
			return new ThetaFilter();
		else
			return FactoryBase/*<TraxTypes>*/::createFilter( id );
	}


};
