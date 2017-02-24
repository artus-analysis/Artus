
#include "Artus/Core/interface/ConsumerBase.h"


ConsumerBaseUntemplated::~ConsumerBaseUntemplated()
{
}

std::string ConsumerBaseUntemplated::GetConsumerId() const
{
	return GetId();
}

