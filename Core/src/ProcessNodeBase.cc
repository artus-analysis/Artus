
#include "Artus/Core/interface/ProcessNodeBase.h"
#include "Artus/Utility/interface/Utility.h"


ProcessNodeBase::~ProcessNodeBase()
{
}

std::string ProcessNodeBase::GetId() const
{
	return Utility::GetClassName(this);
}

