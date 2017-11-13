/* Copyright (c) 2011 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#ifndef KAPPA_FILEINTERFACEBASE_H
#define KAPPA_FILEINTERFACEBASE_H

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <list>

#include <Kappa/DataFormats/interface/Kappa.h>
#include "Artus/KappaTools/interface/BranchHolder.h"

class FileInterfaceBase
{
public:
	enum DataType {STD, GEN, DATA, INVALID} lumiInfoType;
	FileInterfaceBase(int verbose = 2);
	void ClearCache()
	{
		for (std::map<std::string, BranchHolder*>::iterator it = eventBranches.begin(); it != eventBranches.end(); ++it)
			delete it->second;
		eventBranches.clear();
	}
	void Init(TChain* _eventdata, DataType _lumiInfoType);

	void SpeedupTree(long cache = 0);

	inline long long GetEntries()
	{
		return eventdata->GetEntries();
	}
	inline void GetEventEntry(long long entry)
	{
		eventdata->GetEntry(entry);
	}

	// Functions for getting metadata objects
	template<typename T>
	T *GetEvent();

	// Get event content of files
	template<typename T>
	T *GetEvent(const std::string &name, const bool check = true, const bool def = false);
	template<typename T>
	std::vector<std::string> GetEventNames(bool inherited = false);
	template<typename T>
	std::map<std::string, T*> GetEventAll(bool inherited = false);

	bool isMC() const;
	
	TChain* eventdata = nullptr;

protected:
	int verbosity;
	KEventInfo *current_event;
	std::map<std::string, BranchHolder*> eventBranches;

	void *GetInternal(TTree* tree, std::map<std::string, BranchHolder*> &bmap,
		const std::string cname, const std::string &name, const bool check = true);
};

#include "FileInterfaceBase.hxx"

#endif
