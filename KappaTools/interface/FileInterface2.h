/* Copyright (c) 2012 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#ifndef KAPPA_FILEINTERFACE2_H
#define KAPPA_FILEINTERFACE2_H

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <list>

#include "Artus/KappaTools/interface/IOHelper.h"
#include "Artus/KappaTools/interface/ProgressMonitor.h"
#include <Kappa/DataFormats/interface/Kappa.h>
#include "Artus/KappaTools/interface/Directory.h"
#include "Artus/KappaTools/interface/FileInterfaceBase.h"

class FileInterface2 : public FileInterfaceBase
{
public:
	FileInterface2(std::vector<std::string> files, class RunLumiSelector *rls = nullptr,
		bool shuffle = false, int verbose = 2, class ScaleServiceFactory *ss = nullptr, std::string reportFn = "");
	~FileInterface2();
	using FileInterfaceBase::GetEvent;

	// Functions for accessing metadata objects
	void GetLumiEntry();
	void GetLumiEntry(run_id run, lumi_id lumi);
	void GetRunEntry();
	void GetRunEntry(run_id run);

	// Functions for getting metadata objects
	template<typename T>
	T *GetLumi(std::string name, const bool check = true, const bool def = false);
	template<typename T>
	std::vector<std::string> GetLumiNames(bool inherited = false);
	template<typename T>
	std::map<std::string, T*> GetLumiAll(bool inherited = false);

	// Functions for getting metadata objects
	template<typename T>
	T *GetRun(std::string name, const bool check = true, const bool def = false);

	// Functions for getting metadata objects - old style
	template<typename T>
	T *GetEvent(run_id run, lumi_id lumi);
	template<typename T>
	inline T *GetEvent(KEventInfo *meta_event);

protected:
	TChain* lumidata = nullptr;
	TChain* rundata = nullptr;
	
	std::string currentLumiFile;
	std::string currentRunFile;
	std::map<std::string, BranchHolder*> lumiBranches;
	std::map<std::string, BranchHolder*> runBranches;
	std::map<std::pair<run_id, lumi_id>, size_t> lumiIdxMap;
	std::map<run_id, size_t> runIdxMap;
};

#include "FileInterface2.hxx"

#endif
