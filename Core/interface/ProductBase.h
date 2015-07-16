#pragma once

#include <map>
#include "FilterResult.h"

struct ProductBase
{
	// TODO: Is PreviousPipelinesResult really necessary?
	FilterResult PreviousPipelinesResult;
	FilterResult fres;
	std::map<std::string, int> processorRunTime;
};

