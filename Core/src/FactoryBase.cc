
#include "Artus/Core/interface/FactoryBase.h"


FactoryBase::~FactoryBase() {
}

ProducerBaseUntemplated * FactoryBase::createProducer ( std::string const& id ) {
	return nullptr;
}

FilterBaseUntemplated * FactoryBase::createFilter ( std::string const& id ) {
	return nullptr;
}

ConsumerBaseUntemplated * FactoryBase::createConsumer ( std::string const& id ) {
	return nullptr;
}
