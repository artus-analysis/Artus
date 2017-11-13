/* Copyright (c) 2012 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#include "Artus/KappaTools/interface/FileInterface2.h"
#include "Artus/KappaTools/interface/RunLumiReader.h"
#include "Artus/KappaTools/interface/ScaleService.h"
#include <fstream>

using namespace std;

void updateSSF(ScaleServiceFactory *ss, FileInterfaceBase::DataType dt, KLumiInfo *info_lumi)
{
	if (!ss)
		return;
	switch (dt)
	{
		case FileInterfaceBase::DATA:
			ss->registerData(static_cast<KDataLumiInfo*>(info_lumi));
			break;
		case FileInterfaceBase::GEN:
			ss->registerMC(info_lumi);
			break;
		default:
			break;
	}
}

FileInterface2::FileInterface2(std::vector<std::string> files, RunLumiSelector *rls,
	bool shuffle, int verbose, ScaleServiceFactory *ss, std::string reportFn)
	: FileInterfaceBase(verbose), lumidata(new TChain("Lumis")), rundata(new TChain("Runs")), currentLumiFile(), currentRunFile()
{
	eventdata = new TChain("Events");
	
	if (shuffle)
		random_shuffle(files.begin(), files.end());

	DataType dtAll = INVALID;
	std::vector<std::string> usedFiles;
	std::map<run_id, std::set<std::pair<lumi_id, lumi_id> > > usedLumis;

	for (size_t f = 0; f < files.size(); ++f)
	{
		if (verbosity)
			cout << "Loading ... " << files[f] << endl;

		if ((ss != 0) || (reportFn != ""))
		{
			// 1) Load lumi data from single file
			TChain lumicheck("Lumis");
			lumicheck.Add(files[f].c_str());
			BranchHolder bh(&lumicheck, "lumiInfo");
			KLumiInfo *info_lumi = (KLumiInfo*)bh.ptr;

			DataType dt = INVALID;
			if (bh.ClassName() == "KGenRunInfo")
				dt = GEN;
			if (bh.ClassName() == "KDataLumiInfo")
				dt = DATA;
			if (bh.ClassName() == "KLumiInfo")
				dt = STD;

			// 2) acceptance with run selector
			bool accept = (rls == 0);
			for (int l = 0; l < lumicheck.GetEntries(); ++l)
			{
				lumicheck.GetEntry(l);
				if (rls)
				{
					if (rls->accept(info_lumi->nRun, info_lumi->nLumi))
					{
						accept = true;
						usedLumis[info_lumi->nRun].insert(make_pair(info_lumi->nLumi, info_lumi->nLumi));
						updateSSF(ss, dt, info_lumi);
					}
				}
				else
				{
					usedLumis[info_lumi->nRun].insert(make_pair(info_lumi->nLumi, info_lumi->nLumi));
					updateSSF(ss, dt, info_lumi);
				}
			}
			if (!accept)
				continue;

			usedLumis = RunLumiSelector::getMinimalJSON(usedLumis);
			if (dtAll == INVALID)
				dtAll = dt;
			assert(dtAll == dt);
		}

		// 3) add accepted files to chain / persistent lumi info list
		usedFiles.push_back(files[f]);
		
		bool open_succesfully = eventdata->Add(files[f].c_str(),-1);
		if (open_succesfully != 1)
		{
			std::cerr << "Tree " << files[f] << "/Events could not be accessed!" << std::endl;
			exit(1);
		}
	
	}

	if (reportFn != "")
	{
		ofstream uf(string(reportFn + ".usedFiles").c_str(), fstream::out);
		uf << KappaTools::join("\n", usedFiles) << std::endl;
		ofstream fs(string(reportFn + ".json").c_str(), fstream::out);
		RunLumiSelector::printJSON(fs, usedLumis);
	}

	if (usedFiles.size() == 0)
	{
		std::cerr << "No files to process!" << std::endl;
		exit(1);
	}
	Init(eventdata, dtAll);
	if (current_event != 0)
	{
		GetEventEntry(0);
		GetLumiEntry();
		GetRunEntry();
	}
}

FileInterface2::~FileInterface2()
{
	for (std::map<std::string, BranchHolder*>::iterator it = lumiBranches.begin(); it != lumiBranches.end(); ++it)
		delete it->second;
	lumiBranches.clear();
	for (std::map<std::string, BranchHolder*>::iterator it = runBranches.begin(); it != runBranches.end(); ++it)
		delete it->second;
	runBranches.clear();
	ClearCache();
	
	if (eventdata) delete eventdata;
	if (lumidata) delete lumidata;
	if (rundata) delete rundata;
}

void FileInterface2::GetLumiEntry()
{
	GetLumiEntry(current_event->nRun, current_event->nLumi);
}

void FileInterface2::GetLumiEntry(run_id run, lumi_id lumi)
{
	if (eventdata->GetFile()->GetName() != currentLumiFile)
	{
		currentLumiFile = eventdata->GetFile()->GetName();
		
		// Update tree reference of booked variables
		TChain *newLumiData = new TChain("Lumis");
		newLumiData->Add(currentLumiFile.c_str());
		for (std::map<std::string, BranchHolder*>::iterator it = lumiBranches.begin(); it != lumiBranches.end(); ++it)
		{
			it->second->UpdateTree(newLumiData);
		}
		
		if (lumidata) delete lumidata;
		lumidata = newLumiData;
		
		// Rebuild lumi index map
		KLumiInfo *info_lumi = GetLumi<KLumiInfo>("lumiInfo", false, false);
		
		lumiIdxMap.clear();
		for (long long lumiEntry = 0; lumiEntry < lumidata->GetEntries(); ++lumiEntry)
		{
			lumidata->GetEntry(lumiEntry);
			lumiIdxMap[make_pair(info_lumi->nRun, info_lumi->nLumi)] = lumiEntry;
		}
		
	}
	if (lumiIdxMap.count(make_pair(run, lumi)) == 1)
	{
		lumidata->GetEntry(lumiIdxMap[make_pair(run, lumi)]);
	}
	else if (eventdata->GetEntries() > 0)
	{
		std::cerr << "Lumi section " << run << ":" << lumi << " not found or unique!" << std::endl;
		std::cerr << eventdata->GetFile() << " " << eventdata->GetFileNumber() << " " << currentLumiFile << std::endl;
		std::cerr << lumiIdxMap << endl;
		exit(1);
	}
}

void FileInterface2::GetRunEntry()
{
	GetRunEntry(current_event->nRun);
}

void FileInterface2::GetRunEntry(run_id run)
{
	if (eventdata->GetFile()->GetName() != currentRunFile)
	{
		currentRunFile = eventdata->GetFile()->GetName();
		
		// Update tree reference of booked variables
		TChain *newRunData = new TChain("Runs");
		newRunData->Add(currentRunFile.c_str());
		for (std::map<std::string, BranchHolder*>::iterator it = runBranches.begin(); it != runBranches.end(); ++it)
		{
			it->second->UpdateTree(newRunData);
		}
		
		if (rundata) delete rundata;
		rundata = newRunData;
		
		// Rebuild run index map
		KRunInfo *info_run = GetRun<KRunInfo>("runInfo", false, false);
		
		runIdxMap.clear();
		for (long long runEntry = 0; runEntry < rundata->GetEntries(); ++runEntry)
		{
			rundata->GetEntry(runEntry);
			runIdxMap[info_run->nRun] = runEntry;
		}
		
	}
	if (runIdxMap.count(run) == 1)
	{
		rundata->GetEntry(runIdxMap[run]);
	}
	else if (eventdata->GetEntries() > 0)
	{
		std::cerr << "Run " << run << " not found or unique!" << std::endl;
		std::cerr << eventdata->GetFile() << " " << eventdata->GetFileNumber() << " " << currentRunFile << std::endl;
		std::cerr << runIdxMap << endl;
		exit(1);
	}
}
