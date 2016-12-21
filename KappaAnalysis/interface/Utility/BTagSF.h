#pragma once

#include "TRandom3.h"
#include <TFile.h>
#include <TH2.h>
#include <TString.h>
#include <TMath.h>
#include <iostream>

#include "Artus/KappaAnalysis/interface/Utility/BTagCalibrationStandalone.h"

class BTagSF
{
public:
	explicit BTagSF(std::string csvfile, std::string efficiencyfile, std::string btagwp);
	~BTagSF();

	bool isbtagged(double pt, float eta, float csv, Int_t jetflavor,
	               unsigned int btagsys, unsigned int mistagsys, int year, float btagWP);
	double getSFb(double pt, float eta, unsigned int btagsys, int year);
	double getSFc(double pt, float eta, unsigned int btagsys, int year);
	double getSFl(double pt, float eta, unsigned int mistagsys, int year);
	double getMistag(double pt, float eta);
	double getEfficiencyFromFile(int flavour, double pt, float eta);

	enum { kNo, kDown, kUp }; // systematic variations

private:
	TRandom3* randm;
	TFile* effFile;
	BTagCalibration* calib;
	BTagCalibrationReader* reader_mujets;
	BTagCalibrationReader* reader_mujets_up;
	BTagCalibrationReader* reader_mujets_do;
	BTagCalibrationReader* reader_incl;
	BTagCalibrationReader* reader_incl_up;
	BTagCalibrationReader* reader_incl_do;
};
