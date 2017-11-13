/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 *   Manuel Zeise <zeise@cern.ch>
 */

#include "Artus/KappaTools/interface/EventLogger.h"

std::ostream &operator<<(std::ostream &output, const EventID &id)
{
	return output << id.nRun << "\t" << id.nLumi << "\t" << id.nEvent << "\t" << id.nBX;
}
