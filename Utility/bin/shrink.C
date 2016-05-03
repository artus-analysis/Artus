#include "TMath.h"
#include "TLorentzVector.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include <iostream>
//#include "Cintex/Cintex.h"
//
using namespace std;




int main(int argc, char* argv[] ) {

//read in
//evaluate cut
//write out
  std::cout << "Shrinking file " << argv[1]<< " with cut '" << argv[2] << "'" << std::endl;
  TFile *lInput = TFile::Open((argv[1]));
  TTree *lTree = (TTree*) lInput->Get(argv[3]);
  TString expression = argv[2]; //"(nLeptons==2)*(lepton1_charge!=0)*(lepton1_charge==-lepton2_charge)*(z_m>60)*(z_m<120)";
  TTreeFormula *Cut = new TTreeFormula("name", expression, lTree);

  TFile *lOFile = new TFile("slimmed.root", "RECREATE");
  TTree *lOTree = lTree->CloneTree(0);
  int lNEvents = lTree->GetEntries();
  for (Long64_t i0=0; i0<lNEvents;i0++)
  {
    if(i0%1000000==0)
        std::cout << "Event " << i0 << "/" << lNEvents << std::endl;
    lTree->GetEntry(i0);
    float result = (float)Cut->EvalInstance();
    if(result > 0)
      lOTree->Fill();
  }
  lOTree->SetName(lTree->GetName());
  lOTree->Write();
  lOFile->Close();
  std::cout << "Writing output file slimmed.root .... Done!" << std::endl;
  return 0;
}
