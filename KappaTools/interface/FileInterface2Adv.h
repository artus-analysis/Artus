/* Copyright (c) 2012 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#ifndef KAPPA_FILEINTERFACE2ADV_H
#define KAPPA_FILEINTERFACE2ADV_H

#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <list>

#include "Artus/KappaTools/interface/Toolbox/interface/IOHelper.h"
#include "Artus/KappaTools/interface/Toolbox/interface/ProgressMonitor.h"
#include <Kappa/DataFormats/interface/Kappa.h>
#include "Artus/KappaTools/interface/Directory.h"
#include "Artus/KappaTools/interface/FileInterfaceBase.h"

class FileInterface2Adv : public FileInterface2
{
public:
	FileInterface2Adv(std::vector<std::string> files, class RunLumiSelector *rls = 0,
		bool shuffle = false, int verbose = 2, class ScaleServiceFactory *ss = 0, std::string reportFn = "")
		: FileInterface2(files, rls, shuffle, verbose, ss, reportFn), output_file(0), output_event(0), output_lumi(0)
	{
		unsigned long long GB = 1024*1024*1024;
		TTree::SetMaxTreeSize(1000*GB);
	}

	void EnableWriter(std::string fn)
	{
		ROOTGuard guard;
		output_name = fn;
		output_file = new TFile(output_name.c_str(), "RECREATE");
		std::cout << " * Creating output file: " << output_name << std::endl;
	}

	void WriteEvent()
	{
		ROOTGuard guard;
		if (output_file == 0)
			return;
		if (output_event == 0)
			output_event = CloneTreeContent("Events", eventBranches);
		output_event->Fill();
	}

	void WriteLumi()
	{
		ROOTGuard guard;
		if (output_file == 0)
			return;
		if (output_lumi == 0)
			output_lumi = CloneTreeContent("Lumis", lumiBranches);
		output_lumi->Fill();
	}

	void FinalizeWriter()
	{
		ROOTGuard guard;
		if ((output_event != 0) && (output_lumi != 0))
		{
			output_event->Write();
			output_lumi->Write();
		}
		if (output_file != 0)
			output_file->Write();
	}

private:
	std::string output_name;
	TFile *output_file;
	TTree *output_event, *output_lumi;

	TTree *CloneTreeContent(const char *name, std::map<std::string, BranchHolder*> &branchMap)
	{
		ROOTGuard guard;
		output_file->cd();
		std::cout << " * Creating new Tree: " << name << std::endl;
		TTree *result = new TTree(name, name);
//		result->SetDirectory(output_file);

		for (std::map<std::string, BranchHolder*>::const_iterator it = branchMap.begin(); it != branchMap.end(); ++it)
		{

			if (it->second->ptr)
			{
				std::string name = it->first;
				std::string cname = it->second->ClassName();
				std::cout << "\tAdding output branch: " << name << " (" << cname << ")" << std::endl;
				result->Bronch(name.c_str(), cname.c_str(), &(it->second->ptr));
			}
		}
		return result;
	}
};

#endif
