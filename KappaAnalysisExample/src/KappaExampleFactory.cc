
#include "Artus/KappaAnalysisExample/interface/KappaExampleFactory.h"

#include "Artus/KappaAnalysisExample/interface/DiMuonSystemProducer.h"

#include "Artus/KappaAnalysisExample/interface/DiMuonMassFilter.h"
#include "Artus/KappaAnalysisExample/interface/ChargeFilter.h"

#include "Artus/KappaAnalysisExample/interface/DiMuonSystemConsumer.h"


ProducerBaseUntemplated* KappaExampleFactory::createProducer(std::string const& id)
{
	if(id == DiMuonSystemProducer().GetProducerId())
		return new DiMuonSystemProducer();
	else
		return KappaFactory::createProducer( id );
}

FilterBaseUntemplated* KappaExampleFactory::createFilter(std::string const& id)
{
	if(id == DiMuonMassFilter().GetFilterId())
		return new DiMuonMassFilter();
	else if(id == ChargeFilter().GetFilterId())
		return new ChargeFilter();
	else
		return KappaFactory::createFilter( id );
}

ConsumerBaseUntemplated* KappaExampleFactory::createConsumer (std::string const& id)
{
	if(id == DiMuonSystemConsumer().GetConsumerId())
		return new DiMuonSystemConsumer();
	else
		return KappaFactory::createConsumer( id );
}

