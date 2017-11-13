/* Copyright (c) 2011 - All Rights Reserved
 *   Manuel Zeise <zeise@cern.ch>
 */

#ifndef LUMICOUNTER
#define LUMICOUNTER

#include <string>
#include <map>
#include <set>

#include "TH1D.h"
#include "Kappa/DataFormats/interface/Kappa.h"

class LumiCounter
{
private:
	double sumLumi;
	std::set< std::pair<run_id, lumi_id> > alreadyAddedSum;
	std::map< std::string, double > catLumi;
	std::map< std::string, std::set< std::pair<run_id, lumi_id> > > alreadyAddedCat;

public:
	LumiCounter() : sumLumi(0) {}

	void registerCategory(const std::string category)
	{
		std::map< std::string, double >::const_iterator itSearch = catLumi.find(category);
		if (itSearch == catLumi.end())
			catLumi[category] = 0;
	}

	void process(const KDataLumiInfo * lumi, const std::string category = "")
	{
		count(lumi, category);
	}

	void count(const KDataLumiInfo * lumi, const std::string category = "")
	{
		if (!lumi)
			return;

		std::pair<run_id, lumi_id> tmpPair = std::make_pair(lumi->nRun, lumi->nLumi);
		if (alreadyAddedSum.find(tmpPair) == alreadyAddedSum.end())
		{
			sumLumi += lumi->getLumi();
			alreadyAddedSum.insert(tmpPair);
		}

		if (category != "" && alreadyAddedCat[category].find(tmpPair) == alreadyAddedCat[category].end())
		{
			catLumi[category] += lumi->getLumi();
			alreadyAddedCat[category].insert(tmpPair);
		}
	}

	double getLumi(const std::string category = "") const
	{
		if (category == "")
			return sumLumi;

		std::map< std::string, double >::const_iterator itSearch = catLumi.find(category);
		if (itSearch != catLumi.end())
			return itSearch->second;
		else
			return 0;
	}

	void writeHistos() const
	{
		TH1D * hLumi = new TH1D("sumLumi", "all lumi", 1, 0., 1.);
		hLumi->SetBinContent(1, sumLumi);
		hLumi->Write();
		delete hLumi;
		for (std::map< std::string, double >::const_iterator it = catLumi.begin(); it != catLumi.end(); ++it)
		{
			hLumi = new TH1D(it->first.c_str(), it->first.c_str(), 1, 0., 1.);
			hLumi->SetBinContent(1, it->second);
			hLumi->Write();
			delete hLumi;
		}
	}
};
#endif
