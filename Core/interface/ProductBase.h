#pragma once

#include <map>
#include "FilterResult.h"

struct ProductBase
{
	FilterResult PreviousPipelinesResult;
	std::map<std::string, int> processorRunTime;
};

