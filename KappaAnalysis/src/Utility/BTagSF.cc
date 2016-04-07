#include "Artus/KappaAnalysis/interface/Utility/BTagSF.h"


BTagSF::BTagSF(std::string csvfile, std::string efficiencyfile)
{
  
  	TDirectory *savedir(gDirectory);
	TFile *savefile(gFile);
  
	randm = new TRandom3(0);
	calib = new BTagCalibration("csvv2", csvfile);
	
	effFile = new TFile(TString(efficiencyfile));
	if (effFile->IsZombie()) {
		std::cout << "BTagSF: file " << efficiencyfile << " is not found...   quitting " << std::endl;
		exit(-1);
	}
	
	reader_mujets = new BTagCalibrationReader(calib,                // calibration instance
						  BTagEntry::OP_MEDIUM, // operating point
						  "mujets",             // measurement type
						  "central"             // systematics type
						 );
	
	reader_mujets_up = new BTagCalibrationReader(calib, BTagEntry::OP_MEDIUM, "mujets", "up");    // sys up
	reader_mujets_do = new BTagCalibrationReader(calib, BTagEntry::OP_MEDIUM, "mujets", "down");  // sys down
	
	reader_incl = new BTagCalibrationReader(calib,                // calibration instance
						BTagEntry::OP_MEDIUM, // operating point
						"incl",               // measurement type
						"central"             // systematics type
					       );
	
	reader_incl_up = new BTagCalibrationReader(calib, BTagEntry::OP_MEDIUM, "incl", "up");    // sys up
	reader_incl_do = new BTagCalibrationReader(calib, BTagEntry::OP_MEDIUM, "incl", "down");  // sys down
	gDirectory = savedir;
	gFile = savefile;
	
}

BTagSF::~BTagSF()
{
	delete randm;
	delete calib;
	delete effFile;
	delete reader_mujets;
	delete reader_mujets_up;
	delete reader_mujets_do;
	delete reader_incl;
	delete reader_incl_up;
	delete reader_incl_do;
}

bool BTagSF::isbtagged(double pt, float eta, float csv, Int_t jetflavor,
                       unsigned int btagsys, unsigned int mistagsys, int year)
{
	randm->SetSeed(static_cast<int>((eta + 5) * 100000.));
	double randval = randm->Uniform();
	
	float csv_WP = 0.679;
	if(year == 2015)
		csv_WP = 0.8;

	bool btagged = false;
	double sf  = 0.0;
	double eff = 0.0;
	
	double promoteProb_btag = 0.0;  // ~probability to promote to tagged
	double demoteProb_btag  = 0.0;  // ~probability to demote from tagged
	
	// https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideBTagMCTools#Hadron_parton_based_jet_flavour
	// real b-jet
	if (std::abs(jetflavor) == 5)
		sf = getSFb(pt, eta, btagsys, year);
	
	// c-jet
	else if (std::abs(jetflavor) == 4)
		sf = getSFc(pt, eta, btagsys, year);
	
	// light-flavour jet
	else
		sf = getSFl(pt, eta, mistagsys, year);
	
	
	if (sf < 1)
	{
		demoteProb_btag = std::abs(1.0 - sf);
	}
	else
	{
		if (year == 2015)
		{
			eff = getEfficiencyFromFile(jetflavor, pt, eta);
		}
		else
		{
			if (std::abs(jetflavor) == 5)
				eff = 0.719;
			else if (std::abs(jetflavor) == 4)
				eff = 0.192 * sf; // eff_c in MC for CSVM = (-1.5734604211*x*x*x*x +
		                     // 1.52798999269*x*x*x +  0.866697059943*x*x +
		                     // -1.66657942274*x +  0.780639301724), x = 0.679
			else
				eff = getMistag(pt, eta);
		}
		promoteProb_btag = std::abs(sf - 1.0) / ((1.0 / eff) - 1.0);
	}
	
	if (csv > csv_WP) // if tagged
	{
		if (demoteProb_btag > 0. && randval < demoteProb_btag)
			btagged = false;  // demote jet
		else
			btagged = true;   // remains tagged
	}
	else
	{
		if (promoteProb_btag > 0. && randval < promoteProb_btag)
			btagged = true;   // promote jet
		else
			btagged = false;  // remains untagged
	}
	
	return btagged;
}

double BTagSF::getSFb(double pt, float eta, unsigned int btagsys, int year)
{
  if(year == 2015){
	
	float MaxBJetPt = 670.;
	float MinBJetPt = 30.;
	bool DoubleUncertainty = false;
	
	if ((pt > MaxBJetPt) || (pt < MinBJetPt))
	{
	  DoubleUncertainty = true;
	  if (pt > MaxBJetPt)
            pt = MaxBJetPt;
	  else
            pt = MinBJetPt;
	}

	double jet_scalefactor = reader_mujets->eval(BTagEntry::FLAV_B, std::abs(eta), pt);
	double jet_scalefactor_up = reader_mujets_up->eval(BTagEntry::FLAV_B, std::abs(eta), pt);
	double jet_scalefactor_do = reader_mujets_do->eval(BTagEntry::FLAV_B, std::abs(eta), pt);
	
	if (DoubleUncertainty) {
	  jet_scalefactor_up = 2*(jet_scalefactor_up - jet_scalefactor) + jet_scalefactor; 
	  jet_scalefactor_do = 2*(jet_scalefactor_do - jet_scalefactor) + jet_scalefactor; 
	}
	
	if (btagsys == kDown) return jet_scalefactor_do;
	else if (btagsys == kUp) return jet_scalefactor_up;
	else return jet_scalefactor;
  }
  else{
	
	// pT dependent scale factors
	// Tagger: CSVM within 30 < pt < 670 GeV, abs(eta) < 2.4, x = pt
	// SFb = 0.6981*((1.+(0.414063*x))/(1.+(0.300155*x))); (2011)
	// SFb = (0.938887+(0.00017124*x))+(-2.76366e-07*(x*x)); (2012)
	// for pt > 670 (800) GeV: use the SFb value at 670 (800) GeV with twice the
	// quoted uncertainty for 2011 (2012)
	// for pt < 30 (20) GeV: use the SFb value at 30 (20) GeV with a +/-0.12
	// absolute uncertainty (twice the quoted uncertainty) for 2011 (2012)
	// i.e SFb(pt<30) = SFb(pt=30) +/- 0.12, so the relative uncertainty is
	// 0.12/SFb(pt=30) for 2011

	double x = pt;
	if (year==2011 && pt >= 670.0)
		x = 669.9;
	if (year==2011 && pt < 30.0)
		x = 30.0;
	if (year==2012 && pt >= 800.0)
		x = 799.9;
	if (year==2012 && pt < 20.0)
		x = 20.0;

	double SFb = 1.0;
	if (year==2011)
		SFb = 0.6981 * (1.0 + 0.414063 * x) / (1.0 + 0.300155 * x);
	else
	{
		SFb = 0.938887 + 0.00017124 * x - 2.76366e-07 * x * x;
	}
	if (btagsys == kNo)
		return SFb;

	double SFb_error_2011[] = {0.0295675, 0.0295095, 0.0210867, 0.0219349, 0.0227033,
	                           0.0204062, 0.0185857, 0.0256242, 0.0383341, 0.0409675,
	                           0.0420284, 0.0541299, 0.0578761, 0.0655432};
	double ptmin_2011[] = {30, 40, 50, 60, 70, 80, 100, 120, 160, 210, 260, 320, 400, 500};
	double ptmax_2011[] = {40, 50, 60, 70, 80, 100, 120, 160, 210, 260, 320, 400, 500, 670};
	double SFb_error_2012[] = {0.0415707, 0.0204209, 0.0223227, 0.0206655, 0.0199325, 0.0174121,
	                           0.0202332, 0.0182446, 0.0159777, 0.0218531, 0.0204688, 0.0265191,
	                           0.0313175, 0.0415417, 0.0740446, 0.0596716};
	double ptmin_2012[] = {20, 30, 40, 50, 60, 70, 80, 100, 120, 160, 210, 260, 320, 400, 500, 600};
	double ptmax_2012[] = {30, 40, 50, 60, 70, 80, 100, 120, 160, 210, 260, 320, 400, 500, 600, 800};
	

	double SFb_error_x = 0.0;

	unsigned int nbins = 16;
	if(year==2011) nbins = 14;

	for (unsigned int ibin = 0; ibin < nbins; ++ibin)
	{
		if (year==2011)
		{
			if (x >= ptmin_2011[ibin] && x < ptmax_2011[ibin])
				SFb_error_x = SFb_error_2011[ibin];
		}
		else
		{
			if (x >= ptmin_2012[ibin] && x < ptmax_2012[ibin])
				SFb_error_x = SFb_error_2012[ibin];
		}
	}
	if (year==2011)
	{
		if (pt >= 670.0)
			SFb_error_x = 2.0 * 0.0655432;
		if (pt < 30.0)
			SFb_error_x = 0.12;
	}
	else
	{
		if (pt >= 800.0)
			SFb_error_x = 2.0 * 0.0717567;
		if (pt < 20.0)
			SFb_error_x = 2.0 * 0.0554504;
	}

	double scalefactor = SFb;

	if (btagsys == kDown)
		scalefactor = (SFb - SFb_error_x);
	if (btagsys == kUp)
		scalefactor = (SFb + SFb_error_x);

	return scalefactor;
  }
}

double BTagSF::getSFc(double pt, float eta, unsigned int btagsys, int year)
{
  if(year == 2015){
	
	float MaxBJetPt = 670.;
	float MinBJetPt = 30.;
	bool DoubleUncertainty = false;
	
	if ((pt > MaxBJetPt) || (pt < MinBJetPt))
	{
	  DoubleUncertainty = true;
	  if (pt > MaxBJetPt)
            pt = MaxBJetPt;
	  else
            pt = MinBJetPt;
	}
	
	double jet_scalefactor = reader_mujets->eval(BTagEntry::FLAV_C, std::abs(eta), pt); 
	double jet_scalefactor_up = reader_mujets_up->eval(BTagEntry::FLAV_C, std::abs(eta), pt); 
	double jet_scalefactor_do = reader_mujets_do->eval(BTagEntry::FLAV_C, std::abs(eta), pt); 
	
	if (DoubleUncertainty) {
	  jet_scalefactor_up = 2*(jet_scalefactor_up - jet_scalefactor) + jet_scalefactor; 
	  jet_scalefactor_do = 2*(jet_scalefactor_do - jet_scalefactor) + jet_scalefactor; 
	}
	
	if (btagsys == kDown) return jet_scalefactor_do;
	else if (btagsys == kUp) return jet_scalefactor_up;
	else return jet_scalefactor;
  }
  else{

	// SFc = SFb with twice the quoted uncertainty

	double x = pt;
	if (year==2011 && pt >= 670.0)
		x = 669.9;
	if (year==2011 && pt < 30.0)
		x = 30.0;
	if (year==2012 && pt >= 800.0)
		x = 799.9;
	if (year==2012 && pt < 20.0)
		x = 20.0;

	double SFc = 1.0;
	if (year==2011)
	{
		SFc = 0.6981 * (1.0 + 0.414063 * x) / (1.0 + 0.300155 * x);
	}
	else
	{
		SFc = 0.938887 + 0.00017124 * x - 2.76366e-07 * x * x;
	}
	if (btagsys == kNo)
		return SFc;

	double SFb_error_2011[] = {0.0295675, 0.0295095, 0.0210867, 0.0219349, 0.0227033,
	                           0.0204062, 0.0185857, 0.0256242, 0.0383341, 0.0409675,
	                           0.0420284, 0.0541299, 0.0578761, 0.0655432};
	double ptmin_2011[] = {30, 40, 50, 60, 70, 80, 100, 120, 160, 210, 260, 320, 400, 500};
	double ptmax_2011[] = {40, 50, 60, 70, 80, 100, 120, 160, 210, 260, 320, 400, 500, 670};
	double SFb_error_2012[] = {0.0415707, 0.0204209, 0.0223227, 0.0206655, 0.0199325, 0.0174121,
	                           0.0202332, 0.0182446, 0.0159777, 0.0218531, 0.0204688, 0.0265191,
	                           0.0313175, 0.0415417, 0.0740446, 0.0596716};
	double ptmin_2012[] = {20, 30, 40, 50, 60, 70, 80, 100, 120, 160, 210, 260, 320, 400, 500, 600};
	double ptmax_2012[] = {30, 40, 50, 60, 70, 80, 100, 120, 160, 210, 260, 320, 400, 500, 600, 800};

	double SFc_error_x = 0.0;

	unsigned int nbins = 16;
	if(year==2011) nbins = 14;

	for (unsigned int ibin = 0; ibin < nbins; ++ibin)
	{
		if (year==2011)
		{
			if (x >= ptmin_2011[ibin] && x < ptmax_2011[ibin])
				SFc_error_x = 2.0 * SFb_error_2011[ibin];
		}
		else
		{
			if (x >= ptmin_2012[ibin] && x < ptmax_2012[ibin])
				SFc_error_x = 2.0 * SFb_error_2012[ibin];
		}
	}
	if (year==2011)
	{
		if (pt >= 670.0)
			SFc_error_x = 2.0 * 2.0 * 0.0655432;
		if (pt < 30.0)
			SFc_error_x = 2.0 * 0.12;
	}
	else
	{
		if (pt >= 800.0)
			SFc_error_x = 2.0 * 2.0 * 0.0717567;
		if (pt < 20.0)
			SFc_error_x = 2.0 * 2.0 * 0.0554504;
	}

	double scalefactor = SFc;

	if (btagsys == kDown)
		scalefactor = (SFc - SFc_error_x);
	if (btagsys == kUp)
		scalefactor = (SFc + SFc_error_x);

	return scalefactor;
  }
}

double BTagSF::getSFl(double pt, float eta, unsigned int mistagsys, int year)
{
  if(year == 2015){
	
	float MaxLJetPt = 1000.;
	bool DoubleUncertainty = false;
	
	if (pt>MaxLJetPt)
	{
	  DoubleUncertainty = true;
	  pt = MaxLJetPt; 
	}
	
	double jet_scalefactor = reader_incl->eval(BTagEntry::FLAV_UDSG, std::abs(eta), pt); 
	double jet_scalefactor_up = reader_incl_up->eval(BTagEntry::FLAV_UDSG, std::abs(eta), pt); 
	double jet_scalefactor_do = reader_incl_do->eval(BTagEntry::FLAV_UDSG, std::abs(eta), pt); 
	
	if (DoubleUncertainty) {
	  jet_scalefactor_up = 2*(jet_scalefactor_up - jet_scalefactor) + jet_scalefactor; 
	  jet_scalefactor_do = 2*(jet_scalefactor_do - jet_scalefactor) + jet_scalefactor; 
	}
	
	if (mistagsys == kDown) return jet_scalefactor_do;
	else if (mistagsys == kUp) return jet_scalefactor_up;
	else return jet_scalefactor;
  }
  else{

	double x = std::min(pt, year==2011 ? 670.0 : 800.0);

	double SFl = 0;

	if (year==2011)
	{
		if (std::abs(eta) < 0.8f)
		{
			if (mistagsys == kNo)
				SFl = 1.06182 + 0.000617034 * x - 1.5732e-06 * x * x + 3.02909e-10 * x * x * x;
			else if (mistagsys == kDown)
				SFl = 0.972455 + 7.51396e-06 * x + 4.91857e-07 * x * x - 1.47661e-09 * x * x * x;
			else if (mistagsys == kUp)
				SFl = 1.15116 + 0.00122657 * x - 3.63826e-06 * x * x + 2.08242e-09 * x * x * x;
		}
		else if (std::abs(eta) < 1.6f)
		{
			if (mistagsys == kNo)
				SFl = 1.111 - 9.64191e-06 * x + 1.80811e-07 * x * x - 5.44868e-10 * x * x * x;
			else if (mistagsys == kDown)
				SFl = 1.02055 - 0.000378856 * x + 1.49029e-06 * x * x - 1.74966e-09 * x * x * x;
			else if (mistagsys == kUp)
				SFl = 1.20146 + 0.000359543 * x - 1.12866e-06 * x * x + 6.59918e-10 * x * x * x;
		}
		else if (std::abs(eta) < 2.4f)
		{
			if (mistagsys == kNo)
				SFl = 1.08498 - 0.000701422 * x + 3.43612e-06 * x * x - 4.11794e-09 * x * x * x;
			else if (mistagsys == kDown)
				SFl = 0.983476 - 0.000607242 * x + 3.17997e-06 * x * x - 4.01242e-09 * x * x * x;
			else if (mistagsys == kUp)
				SFl = 1.18654 - 0.000795808 * x + 3.69226e-06 * x * x - 4.22347e-09 * x * x * x;
		}
	}
	else
	{
		if (std::abs(eta) < 0.8f)
		{
			if (mistagsys == kNo)
				SFl = 1.07541 + 0.00231827 * x - 4.74249e-06 * x * x + 2.70862e-09 * x * x * x;
			else if (mistagsys == kDown)
				SFl = 0.964527 + 0.00149055 * x - 2.78338e-06 * x * x + 1.51771e-09 * x * x * x;
			else if (mistagsys == kUp)
				SFl = 1.18638 + 0.00314148 * x - 6.68993e-06 * x * x + 3.89288e-09 * x * x * x;
		}
		else if (std::abs(eta) < 1.6f)
		{
			if (mistagsys == kNo)
				SFl = 1.05613 + 0.00114031 * x - 2.56066e-06 * x * x + 1.67792e-09 * x * x * x;
			else if (mistagsys == kDown)
				SFl = 0.946051 + 0.000759584 * x - 1.52491e-06  * x * x + 9.65822e-10 * x * x * x;
			else if (mistagsys == kUp)
				SFl = 1.16624 + 0.00151884 * x - 3.59041e-06 * x * x + 2.38681e-09 * x * x * x;
		}
		else if (std::abs(eta) < 2.4f)
		{
			if (mistagsys == kNo)
				SFl = 1.05625 + 0.000487231 * x - 2.22792e-06 * x * x + 1.70262e-09 * x * x * x;
			else if (mistagsys == kDown)
				SFl = 0.956736 + 0.000280197 * x - 1.42739e-06 * x * x + 1.0085e-09 * x * x * x;
			else if (mistagsys == kUp)
				SFl = 1.15575 + 0.000693344* x - 3.02661e-06 * x * x + 2.39752e-09 * x * x * x;
		}
	}

	return SFl;
  }
}

double BTagSF::getMistag(double pt, float eta)
{
	double x = std::min(pt, 670.0);

	if (std::abs(eta) < 0.8f)
		return 0.00967751 + 2.54564e-05 * x - 6.92256e-10 * x * x;
	if (std::abs(eta) < 1.6f)
		return 0.00974141 + 5.09503e-05 * x + 2.0641e-08 * x * x;
	if (std::abs(eta) < 2.4f)
		return 0.013595 + 0.000104538 * x - 1.36087e-08 * x * x;
	return 0.0;
}

double BTagSF::getEfficiencyFromFile(int flavour, double pt, float eta)
{
	TH2D * effHisto;
	double eff = 0.0;
	
	if (flavour == 5)
	{
		effHisto = (TH2D*) effFile->Get("btag_eff_b");
	}
	else if (flavour == 4)
	{
		effHisto = (TH2D*) effFile->Get("btag_eff_c");
	}
	else
	{
		effHisto = (TH2D*) effFile->Get("btag_eff_oth");
	}
	
	if (pt > effHisto->GetXaxis()->GetBinLowEdge(effHisto->GetNbinsX()+1))
		eff = effHisto->GetBinContent(effHisto->GetNbinsX(), effHisto->GetYaxis()->FindBin(std::abs(eta)));
	else
		eff = effHisto->GetBinContent(effHisto->GetXaxis()->FindBin(pt), effHisto->GetYaxis()->FindBin(std::abs(eta)));
	
	return eff;
}
