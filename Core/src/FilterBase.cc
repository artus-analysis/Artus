
#include "Artus/Core/interface/FilterBase.h"


FilterBaseUntemplated::~FilterBaseUntemplated()
{
}

std::string FilterBaseUntemplated::GetFilterId() const
{
	return GetId();
}

