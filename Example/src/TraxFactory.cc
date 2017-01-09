
#include "Artus/Example/interface/TraxFactory.h"

// producer
#include "Artus/Example/interface/PtCorrectionProducer.h"
#include "Artus/Example/interface/PtCorrectionProducerLocal.h"

// filter
#include "Artus/Example/interface/PtFilter.h"
#include "Artus/Example/interface/ThetaFilter.h"

// consumer
#include "Artus/Example/interface/MeanPtConsumer.h"
#include "Artus/Example/interface/TraxNtupleConsumer.h"
#include "Artus/Example/interface/CutFlowConsumer.h"


TraxFactory::TraxFactory() :
		FactoryBase()
{
}

TraxFactory::~TraxFactory()
{
}

ProducerBaseUntemplated* TraxFactory::createProducer(std::string const& id)
{
	if ( PtCorrectionProducer().GetProducerId() == id )
		return new PtCorrectionProducer();
	else if ( PtCorrectionProducerLocal().GetProducerId() == id )
		return new PtCorrectionProducerLocal();
	else
		return FactoryBase::createProducer( id );
}

ConsumerBaseUntemplated* TraxFactory::createConsumer(std::string const& id)
{
	/*if ( MeanPtConsumer().GetConsumerId() == id )
		return new MeanPtConsumer();
	else if ( TraxNtupleConsumer().GetConsumerId() == id )
		return new TraxNtupleConsumer();
	else if ( CutFlowConsumer().GetConsumerId() == id )
		return new CutFlowConsumer();
	else*/
		return FactoryBase::createConsumer( id );
}

FilterBaseUntemplated* TraxFactory::createFilter(std::string const& id)
{
	if ( PtFilter().GetFilterId() == id )
		return new PtFilter();
	else if ( ThetaFilter().GetFilterId() == id )
		return new ThetaFilter();
	else
		return FactoryBase::createFilter( id );
}

