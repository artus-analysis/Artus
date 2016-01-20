
#include "Artus/KappaAnalysisExample/interface/KappaExampleFactory.h"


ProducerBaseUntemplated* KappaExampleFactory::createProducer(std::string const& id)
{
	return KappaFactory::createProducer( id );
}

FilterBaseUntemplated* KappaExampleFactory::createFilter(std::string const& id)
{
	return KappaFactory::createFilter( id );
}

ConsumerBaseUntemplated* KappaExampleFactory::createConsumer (std::string const& id)
{
	return KappaFactory::createConsumer( id );
}

