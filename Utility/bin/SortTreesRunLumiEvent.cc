
#include <iostream>
#include <string>
#include <map>

#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <TChain.h>

#include "boost/program_options.hpp"


int main(int argc, char* argv[]) {
	
	std::vector<std::string> inputFiles;
	std::string treeName;
	std::string outputFile = "output.root";

	boost::program_options::options_description help_config("Help");
	help_config.add_options()
		("help,h", "Show the help message");
	boost::program_options::options_description config("Configuration");
	config.add_options()
		("input,i", boost::program_options::value<std::vector<std::string> >(&inputFiles)->default_value(inputFiles, ""),
		"Input files.")
		("tree,t", boost::program_options::value<std::string>(&treeName)->default_value(treeName),
		"Name of the tree to be sorted.")
		("output,o", boost::program_options::value<std::string>(&outputFile)->default_value(outputFile),
		"Output file.");

	boost::program_options::variables_map vm;
	boost::program_options::store(
			boost::program_options::command_line_parser(argc, argv).options(help_config).allow_unregistered().run(),
			vm
	);
	boost::program_options::notify(vm);
	if (vm.count("help")) {
		std::cout << config << std::endl;
		return 1;
	}
	boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(config).run(), vm);
	boost::program_options::notify(vm);
	
	assert(! treeName.empty());
	if (inputFiles.size() == 0)
	{
		return 0;
	}
	
	TChain* inputTree = new TChain();
	for (std::string inputFile : inputFiles) {
		inputTree->Add(std::string(inputFile+"/"+treeName).c_str());
	}
	
	// https://root.cern.ch/root/roottalk/roottalk01/3646.html
	//Int_t nEntries = inputTree->GetEntries();
	Int_t nEntries = inputTree->Draw("event", "", "goff");
	Int_t* indices = new Int_t[nEntries];
	TMath::Sort(nEntries, inputTree->GetV1(), indices);
	
	TFile rootOutputFile(outputFile.c_str(), "RECREATE");
	TTree* outputTree = inputTree->CloneTree(0);
	for (Long64_t entry = 0; entry < nEntries; ++entry)
	{
		inputTree->GetEntry(indices[entry]);
		outputTree->Fill();
	}
	
	rootOutputFile.Write();
	rootOutputFile.Close();
}
