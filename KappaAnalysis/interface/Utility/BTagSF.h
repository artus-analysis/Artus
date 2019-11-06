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
	BTagSF();
	BTagSF(BTagSF const&) = default;
	BTagSF(std::string csvfile, std::string efficiencyfile);
	BTagSF(std::string csvfile, std::string efficiencyfile, std::string btagwp);
	~BTagSF();

	void initBtagwp(std::string btagwp);

	bool isbtagged(double pt, float eta, float csv, Int_t jetflavor,
	               unsigned int btagsys, unsigned int mistagsys, int year, float btagWP) const;
	double getSFb(double pt, float eta, unsigned int btagsys, int year) const;
	double getSFc(double pt, float eta, unsigned int btagsys, int year) const;
	double getSFl(double pt, float eta, unsigned int mistagsys, int year) const;
	double getMistag(double pt, float eta) const;
	double getEfficiencyFromHistogram(int flavour, double pt, float eta) const;

	enum { kNo, kDown, kUp }; // systematic variations

private:
	mutable TRandom3 randm;
	BTagCalibration calib;
	TH2D* effHisto_b = nullptr;
	TH2D* effHisto_c = nullptr;
	TH2D* effHisto_oth = nullptr;
	BTagCalibrationReader reader_mujets;
	BTagCalibrationReader reader_mujets_up;
	BTagCalibrationReader reader_mujets_do;
	BTagCalibrationReader reader_incl;
	BTagCalibrationReader reader_incl_up;
	BTagCalibrationReader reader_incl_do;
};
