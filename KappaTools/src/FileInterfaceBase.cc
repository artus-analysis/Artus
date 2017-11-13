/* Copyright (c) 2011 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#include "Artus/KappaTools/interface/FileInterfaceBase.h"

#include "TEnv.h"

using namespace std;

FileInterfaceBase::FileInterfaceBase(int verbose) :
	lumiInfoType(STD), eventdata(0), verbosity(verbose)
{
}

void FileInterfaceBase::Init(TChain* _eventdata, FileInterfaceBase::DataType _lumiInfoType)
{
	eventdata = _eventdata;
	lumiInfoType = _lumiInfoType;
	current_event = GetEvent<KEventInfo>("eventInfo", false);
	assert((current_event != 0) || (eventdata->GetEntries() == 0));
	switch (lumiInfoType)
	{
		case GEN:
			cout << endl << "Data source: Monte Carlo" << endl;
			break;
		case DATA:
			cout << endl << "Data source: Detector" << endl;
			break;
		default:
			cout << endl << "Data source: default" << endl;
	}
}

void FileInterfaceBase::SpeedupTree(long cache)
{
	if (cache > 0)
		eventdata->SetCacheSize(cache);
	TObjArray *eventBranches = eventdata->GetListOfBranches();
	if (eventBranches != 0)
	{
		for (int i = 0; i < eventBranches->GetEntries(); ++i)
		{
			TBranch *b = dynamic_cast<TBranch*>(eventBranches->At(i));
			if (b->GetAddress() == 0)
			{
				UInt_t found = 0;
				string btype = b->GetClassName();
				string bname;
				if (btype.find("vector") == 0)
					bname = string(b->GetName()) + ".*";
				else
					bname = b->GetName();
				eventdata->SetBranchStatus(bname.c_str(), 0, &found);
			}
			else
				if (cache > 0)
					eventdata->AddBranchToCache(b); // TODO: still needed?
		}
	}
	eventdata->AddBranchToCache("*", true);
	gEnv->SetValue("TFile.AsyncPrefetching", 1);
}

void *FileInterfaceBase::GetInternal(TTree* tree, std::map<std::string, BranchHolder*> &bmap,
	const std::string cname, const std::string &name, const bool check)
{
	// First time access
	if (bmap[name] == 0)
		bmap[name] = new BranchHolder(tree, name, cname, check);
	// Only check class in subsequent access
	if (bmap[name]->ptr && CheckType(cname, bmap[name]->ClassName()))
		return bmap[name]->ptr;
	return 0;
}

bool FileInterfaceBase::isMC() const
{
	return (lumiInfoType == GEN);
}
