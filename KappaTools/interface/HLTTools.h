
#ifndef KAPPA_HLTTOOLS_H
#define KAPPA_HLTTOOLS_H

#include <Kappa/DataFormats/interface/Kappa.h>

class HLTTools
{
private:
	mutable std::map<std::string, std::string> nameCache;
	mutable std::map<std::string, size_t> posCache;
	KLumiInfo * lumiInfo;
	unsigned int currentNLumi; // trigger will not change in the same lumi section
	unsigned int currentNRun; // also check for the rare cases if file will change, but lumi is kept
	bool refill_hlt; // will be used for the refill of the chaces. Which takes some time O(100 ms)
	
	
public:
	HLTTools(KLumiInfo * lumiInfo = nullptr);
	void setLumiInfo(KLumiInfo * lumiInfo);
	bool isHLTDefect() const;
	bool isPrescaled(const std::string &hltName) const;
	bool isAvailable(const std::string &hltName) const;
	unsigned int getPrescale(const std::string &hltName) const;
	std::string getHLTName(const std::string &hltName) const;
	size_t getHLTPosition(const std::string &hltName) const;

};

#endif
