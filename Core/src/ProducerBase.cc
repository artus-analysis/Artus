
#include "Artus/Core/interface/ProducerBase.h"


ProducerBaseUntemplated::~ProducerBaseUntemplated()
{
}

std::string ProducerBaseUntemplated::GetProducerId() const
{
	return GetId();
}

