/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#include "Artus/KappaTools/interface/DisplayTools.h"
#include <bitset>

void displayWeight(KGenRunInfo *infoLumi, KGenEventInfo *infoEvent)
{
	std::cout << "Ext: " << infoLumi->xSectionExt << " Int: " << infoLumi->xSectionInt
		<< " W: " << infoEvent->weight << std::endl;
}

void displayBits(KEventInfo *info)
{
	std::cout << "      " << " 6 6 5 5 5 5 5 4 4 4 4 4 3 3 3 3 3 2 2 2 2 2 1 1 1 1 1" << std::endl;
	std::cout << "      " << " 2 0 8 6 4 2 0 8 6 4 2 0 8 6 4 2 0 8 6 4 2 0 8 6 4 2 0 8 6 4 2 0" << std::endl;
	std::cout << "      " << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << std::endl;
	std::cout << "  L1: " << std::bitset<64>(info->bitsL1) << std::endl;
	std::cout << " HLT: ";
	for(auto bit : info->bitsHLT)
		std::cout << bit;
	std::cout << std::endl;
	std::cout << "Flag: " << std::bitset<64>(info->bitsUserFlags) << std::endl;
}

void displayHLT(KLumiInfo *infoLumi, KEventInfo *infoEvent)
{
	for (size_t hltIdx = 0; hltIdx < infoLumi->hltNames.size(); ++hltIdx)
		if (infoEvent->bitsHLT[hltIdx])
			std::cout << hltIdx << ":" << infoLumi->hltNames[hltIdx] << " ";
		std::cout << std::endl;
}
