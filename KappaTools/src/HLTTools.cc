
#include "Artus/KappaTools/interface/HLTTools.h"

#include <boost/regex.hpp>


HLTTools::HLTTools(KLumiInfo * lumiInfo)
{
	currentNLumi = 0; // just a default value, will be overwritten by first lumi
	currentNRun = 0;// just a default value, will be overwritten by first lumi
	refill_hlt = true; // Allways fill the hlt infos in the first run
	if (lumiInfo)
	{
		setLumiInfo(lumiInfo);
	}
}

void HLTTools::setLumiInfo(KLumiInfo * lumiInfo)
{
	this->lumiInfo = lumiInfo;
	
	if ((lumiInfo->nLumi != currentNLumi) || (lumiInfo->nRun != currentNRun))
	{
		nameCache.clear();
		posCache.clear();
		for (size_t idx = 0; idx < lumiInfo->hltNames.size(); ++idx)
		{
			posCache[lumiInfo->hltNames[idx]] = idx;
			nameCache[lumiInfo->hltNames[idx]] = lumiInfo->hltNames[idx];
		}
		refill_hlt = false;
	}
	
	currentNLumi = lumiInfo->nLumi;
	currentNRun = lumiInfo->nRun;
}

bool HLTTools::isHLTDefect() const
{
	if (!lumiInfo)
		return true;
	for (size_t idx = 1; idx < lumiInfo->hltNames.size(); idx++)
	{
		if (!lumiInfo->hltPrescales[idx])
		{
			return true;
		}
	}
	return false;
}

bool HLTTools::isPrescaled(const std::string &hltName) const
{
	return (getPrescale(hltName) > 1);
}

bool HLTTools::isAvailable(const std::string &hltName) const
{
	return (getHLTName(hltName) != "");
}

unsigned int HLTTools::getPrescale(const std::string &hltName) const
{
	if (!lumiInfo)
		return 0;
	std::string tmpHLTName = getHLTName(hltName);
	if (tmpHLTName == "")
		return 0;
	return lumiInfo->hltPrescales[posCache[tmpHLTName]];
}

std::string HLTTools::getHLTName(const std::string &hltName) const
{
	if (hltName.empty())
	{
		return hltName;
	}
	
	if (nameCache.count(hltName) > 0)
	{
		return nameCache[hltName];
	}

	if (!lumiInfo)
	{
		return "";
	}

	boost::regex pattern(hltName+"(_v[[:digit:]]+)?$", boost::regex::icase | boost::regex::extended);
	for (std::vector<std::string>::const_iterator curIt = lumiInfo->hltNames.begin();
	     curIt != lumiInfo->hltNames.end(); ++curIt)
	{
		if (boost::regex_search(*curIt, pattern))
		{
			nameCache[hltName] = *curIt;
			return *curIt;
		}
	}
	return "";
}

size_t HLTTools::getHLTPosition(const std::string &hltName) const
{
	std::string tmpHLTName = getHLTName(hltName);
	if (tmpHLTName == "")
		return 0;
	return posCache[tmpHLTName];
}

