/* Copyright (c) 2010 - All Rights Reserved
 *   Andreas Oehler <andreasoehler@gmx.net>
 *   Armin Burgmeier <burgmeier@ekp.uni-karlsruhe.de>
 *   Fred Stober <stober@cern.ch>
 *   Manuel Zeise <zeise@cern.ch>
 */

#include "Artus/KappaTools/interface/FileInterface.h"

using namespace std;

FileInterface::FileInterface(vector<string> files, bool shuffle, int verbose) :
	eventdata("Events"), lumidata("Lumis"), rundata("Runs"), verbosity(verbose)
{
	if (shuffle)
		random_shuffle(files.begin(), files.end());
	for (size_t i = 0; i < files.size(); ++i)
	{
		if (verbosity)
			cout << "Loading ... " << files[i] << endl;
		eventdata.Add(files[i].c_str());
		lumidata.Add(files[i].c_str());
		rundata.Add(files[i].c_str());
	}

	TBranch *b = lumidata.GetBranch("lumiInfo");
	if (b)
	{
		if (string(b->GetClassName()) == "KDataLumiInfo")
		{
			Init(&eventdata, DATA);
			lumimap_data = GetLumis<KDataLumiInfo>();
		}
		else if (string(b->GetClassName()) == "KLumiInfo")
		{
			Init(&eventdata, STD);
			lumimap_std = GetLumis<KLumiInfo>();
		}
	}
	else
	{
		std::cerr << "Can't determine file type (Data/MC/Standard)" << std::endl;
		exit(1);
	}
}

FileInterface::~FileInterface()
{
	ClearCache();
}

template<typename T>
inline bool isCompatible(const std::map<std::pair<run_id, lumi_id>, T> &lumimap, unsigned int minRun, unsigned int maxRun)
{
	for (typename std::map<std::pair<run_id, lumi_id>, T>::const_iterator it = lumimap.begin(); it != lumimap.end(); ++it)
		if ((it->first.first >= minRun || minRun == 0) && (it->first.first <= maxRun || maxRun == 0))
			return true;
	return false;
}

bool FileInterface::isCompatible(unsigned int minRun, unsigned int maxRun)
{
	switch (lumiInfoType)
	{
	case GEN:
		return ::isCompatible(lumimap_mc, minRun, maxRun);
	case STD:
		return ::isCompatible(lumimap_std, minRun, maxRun);
	case DATA:
		return ::isCompatible(lumimap_data, minRun, maxRun);
	default:
		break;
	}
	return false;
}

template<typename T>
inline std::vector<std::pair<run_id, lumi_id> > GetRunLumis(const std::map<std::pair<run_id, lumi_id>, T> &lumimap)
{
	std::vector<std::pair<run_id, lumi_id> > result;
	for (typename std::map<std::pair<run_id, lumi_id>, T>::const_iterator it = lumimap.begin(); it != lumimap.end(); ++it)
		result.push_back(it->first);
	return result;
}

std::vector<std::pair<run_id, lumi_id> > FileInterface::GetRunLumis() const
{
	switch (lumiInfoType)
	{
	case GEN:
		return ::GetRunLumis(lumimap_mc);
	case STD:
		return ::GetRunLumis(lumimap_std);
	case DATA:
		return ::GetRunLumis(lumimap_data);
	default:
		break;
	}
	return std::vector<std::pair<run_id, lumi_id> >();
}

template<>
KLumiInfo *FileInterface::GetEvent(run_id run, lumi_id lumi)
{
	switch (lumiInfoType)
	{
	case GEN:
		//return &(lumimap_mc[std::make_pair(run, lumi)]);
		return &(lumimap_std[std::make_pair(run, lumi)]);
	case STD:
		return &(lumimap_std[std::make_pair(run, lumi)]);
	case DATA:
		return &(lumimap_data[std::make_pair(run, lumi)]);
	default:
		break;
	}
	return 0;
}

template<>
KGenRunInfo *FileInterface::GetEvent(run_id run, lumi_id lumi)
{
	if (lumiInfoType == GEN)
		return &(lumimap_mc[std::make_pair(run, lumi)]);
	return 0;
}

template<>
KDataLumiInfo *FileInterface::GetEvent(run_id run, lumi_id lumi)
{
	if (lumiInfoType == DATA)
		return &(lumimap_data[std::make_pair(run, lumi)]);
	return 0;
}
