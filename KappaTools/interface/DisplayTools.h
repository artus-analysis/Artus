/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#ifndef KAPPA_DISPLAYTOOLS_H
#define KAPPA_DISPLAYTOOLS_H

#include <Kappa/DataFormats/interface/Kappa.h>
#include <Kappa/DataFormats/interface/KDebug.h>
#include <stdlib.h>
#include <stddef.h>
#include <cstring>

void displayWeight(KGenRunInfo *infoLumi, KGenEventInfo *infoEvent);
void displayBits(KEventInfo *info);
void displayHLT(KLumiInfo *infoLumi, KEventInfo *infoEvent);

template<typename T>
void displayLVs(const std::vector<T> &vec, size_t maxN = 0)
{
	if (maxN > 0)
		maxN = min(maxN, vec.size());
	else
		maxN = vec.size();
	for (size_t j = 0; j < maxN; ++j)
		std::cout << vec[j].p4 << std::endl;
}

template<typename T>
void displayJets(const std::vector<T> &vec, size_t maxN = 0)
{
	if (maxN > 0)
		maxN = min(maxN, vec.size());
	else
		maxN = vec.size();
	for (size_t j = 0; j < maxN; ++j)
		std::cout << vec[j] << std::endl;
}

template<typename T1, typename T2>
void displayMatching(const T1 *baseObj, const T2 *targetObj, const std::vector<int> &match)
{
	for (size_t j = 0; j < match.size(); ++j)
	{
		const int k = match[j];
		std::cout << j << "->" << k << "\t";
		if (k < 0)
		{
			std::cout << std::endl;
			continue;
		}
		std::cout << targetObj->at(j).p4 << " -> " << baseObj->at(k).p4 << std::endl;
	}
}

#endif
