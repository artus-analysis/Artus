/* Copyright (c) 2012 - All Rights Reserved
 *   Manuel Zeise <zeise@cern.ch>
 */

#include "Artus/KappaTools/interface/PUReweighter.h"

PUReweighter::PUReweighter(std::vector<std::string> inputFilesData, std::vector<std::string> inputFilesMC, float scaleFactor)
{
	assert(inputFilesData.size()>0);
	initTruthMatrix(inputFilesData, inputFilesMC);
	initApproxMatrix(inputFilesData, scaleFactor);
}

void PUReweighter::initTruthMatrix(std::vector<std::string> inputFilesData, std::vector<std::string> inputFilesMC)
{
	puDistrData = 0; //new TH1F("puDistrData", "", 36, -0.5, 35.5);
	for (std::vector<std::string>::const_iterator it = inputFilesData.begin(); it != inputFilesData.end(); ++it)
	{
		std::cout << *it << "\n";
		TFile *file_ = TFile::Open(it->c_str());
		TH1F *tmpDistr = (TH1F *) file_->Get("pileup");
		if (puDistrData == 0)
		{
			puDistrData = new TH1F("puDistrData", "", tmpDistr->GetNbinsX(), tmpDistr->GetBinLowEdge(1), tmpDistr->GetBinLowEdge(tmpDistr->GetNbinsX()+1));
			puDistrData->SetDirectory(0);
		}
		puDistrData->Add(tmpDistr);
		file_->Close();
		delete file_;
	}
	
	puDistrMC = 0; //new TH1F("puDistrMC", "", 36, -0.5, 35.5);

	for (std::vector<std::string>::const_iterator it = inputFilesMC.begin(); it != inputFilesMC.end(); ++it)
	{
		std::cout << *it << "\n";
		TFile *file_ = TFile::Open(it->c_str());
		TH1F *tmpDistr = (TH1F *) file_->Get("pileup");
		if (puDistrMC == 0)
		{
			puDistrMC = new TH1F("puDistrMC", "", tmpDistr->GetNbinsX(), tmpDistr->GetBinLowEdge(1), tmpDistr->GetBinLowEdge(tmpDistr->GetNbinsX()+1));
			puDistrMC->SetDirectory(0);
		}
		puDistrMC->Add(tmpDistr);
		file_->Close();
		delete file_;
	}

	assert(puDistrData);
	assert(puDistrMC);

	if (puDistrData->GetNbinsX()>100)
		puDistrData->Rebin(10);
	if (puDistrMC->GetNbinsX()>100)
		puDistrMC->Rebin(10);

	puDistrData->Scale(1.0 / puDistrData->Integral());
	puDistrMC->Scale(1.0 / puDistrMC->Integral());

	
	/*
	Double_t probdistFall11[50] = {
		0.00124666,
		0.00623184,
		0.0165881,
		0.0337864,
		0.0487338,
		0.0646679,
		0.0703802,
		0.0725553,
		0.0743569,
		0.0663082,
		0.0591014,
		0.0423741,
		0.0465633,
		0.041351,
		0.0446009,
		0.0419762,
		0.0329596,
		0.0291112,
		0.0325672,
		0.0261565,
		0.0211431,
		0.0257527,
		0.0219478,
		0.0155669,
		0.0174131,
		0.0128752,
		0.0078817,
		0.00728786,
		0.00354981,
		0.00518267,
		0.002932,
		0.00311129,
		0.00146143,
		0.000618991,
		0.000412325,
		0.000412459,
		0.,
		0.000412593,
		8.80863e-06,
		0.000412493,
		0.,
		0.,
		0.,
		0.,
		0.,
		0.,
		0.,
		0.,
		0.,
		0.
	};

	Double_t probdistFall11[50] = {
		0.003388501,
		0.010357558,
		0.024724258,
		0.042348605,
		0.058279812,
		0.068851751,
		0.072914824,
		0.071579609,
		0.066811668,
		0.060672356,
		0.054528356,
		0.04919354,
		0.044886042,
		0.041341896,
		0.0384679,
		0.035871463,
		0.03341952,
		0.030915649,
		0.028395374,
		0.025798107,
		0.023237445,
		0.020602754,
		0.0180688,
		0.015559693,
		0.013211063,
		0.010964293,
		0.008920993,
		0.007080504,
		0.005499239,
		0.004187022,
		0.003096474,
		0.002237361,
		0.001566428,
		0.001074149,
		0.000721755,
		0.000470838,
		0.00030268,
		0.000184665,
		0.000112883,
		6.74043E-05,
		3.82178E-05,
		2.22847E-05,
		1.20933E-05,
		6.96173E-06,
		3.4689E-06,
		1.96172E-06,
		8.49283E-07,
		5.02393E-07,
		2.15311E-07,
		9.56938E-08
	};
	*/

}

void PUReweighter::initApproxMatrix(std::vector<std::string> inputFiles, float scaleFactor)
{
	if (inputFiles.empty())
	{
		std::cout << "You did not provide any input files to PUReweighter::initApproxMatrix()!" << std::endl;
		std::cout << "Please check. Terminating." << std::endl;
		return;
	}

	TH1F *Data_distr_ = nullptr;

	for (std::vector<std::string>::const_iterator it = inputFiles.begin(); it != inputFiles.end(); ++it)
	{
		std::cout << *it << "\n";
		TFile *file_ = TFile::Open(it->c_str());
		TH1F *tmpDistr = (TH1F *) file_->Get("pileup");
		if (Data_distr_ == nullptr)
		{
			Data_distr_ = new TH1F("Data_distr_", "", tmpDistr->GetNbinsX(), tmpDistr->GetBinLowEdge(1), tmpDistr->GetBinLowEdge(tmpDistr->GetNbinsX()+1));
			Data_distr_->SetDirectory(0);
		}
		Data_distr_->Add(tmpDistr);
		file_->Close();
		delete file_;
	}

	if (Data_distr_ == nullptr)
	{
		std::cout << "Data distribution could not be initialized in PUReweighter::initApproxMatrix()." << std::endl;
		std::cout << "Please check. Terminating." << std::endl;
		return;
	}

    Data_distr_->Scale(1.0 / Data_distr_->Integral());

    TH1F *MC_distr_ = new TH1F("MC_distr", "MC_distr", 25, -0.5, 24.5);
    Double_t probdistFlat10[25] = {
		0.0698146584,
		0.0698146584,
		0.0698146584,
		0.0698146584,
		0.0698146584,
		0.0698146584,
		0.0698146584,
		0.0698146584,
		0.0698146584,
		0.0698146584,
		0.0698146584,
		0.0630151648,
		0.0526654164,
		0.0402754482,
		0.0292988928,
		0.0194384503,
		0.0122016783,
		0.007207042,
		0.004003637,
		0.0020278322,
		0.0010739954,
		0.0004595759,
		0.0002229748,
		0.0001028162,
		4.58337152809607E-05
    };
    for (size_t idx = 0; idx < 25; ++idx)
		MC_distr_->SetBinContent(idx + 1, probdistFlat10[idx]);


    // Scale factor is used to shift target distribution (i.e. luminosity scale)  1. = no shift

    //create histogram to write output weights, save pain of generating them again...

    using std::min;

    if (MC_distr_->GetEntries() == 0) {
		std::cout << " MC and Data distributions are not initialized! You must call the Lumi3DReWeighting constructor. " << std::endl;
    }

    // arrays for storing number of interactions

    double MC_ints[50][50][50];
    double Data_ints[50][50][50];

	for (int i = 0; i < 50; i++)
		for (int j = 0; j < 50; j++)
			for (int k = 0; k < 50; k++)
			{
				MC_ints[i][j][k] = 0.;
				Data_ints[i][j][k] = 0.;
			}

    double factorial[51];
    double PowerSer[50];
    double base = 1.;

    factorial[0] = 1.;
    PowerSer[0] = 1.;

    for (int i = 1; i < 51; ++i) {
	base = base * float (i);
	factorial[i] = base;
    }


    double x;
    double xweight;
    double probi, probj, probk;
    double Expval, mean;
    int xi;

    // Get entries for Data, MC, fill arrays:

    int NMCbin = MC_distr_->GetNbinsX();

    for (int jbin = 1; jbin < NMCbin + 1; jbin++)
    {
		x = MC_distr_->GetBinCenter(jbin);
		xweight = MC_distr_->GetBinContent(jbin);	//use as weight for matrix


		//for Summer 11, we have this int feature:
		xi = int (x);

		// Generate Poisson distribution for each value of the mean

		mean = double (xi);

		if (mean < 0.) {
			std::cout << " Your histogram generates MC luminosity values less than zero!" << " Please Check.  Terminating." << std::endl;
			return;
		}

		Expval = exp(-1. * mean);

		base = 1.;

		for (int i = 1; i < 50; ++i)
		{
			base = base * mean;
			PowerSer[i] = base;	// PowerSer is mean^i
		}

		// compute poisson probability for each Nvtx in weight matrix

		for (int i = 0; i < 50; i++) {
			probi = PowerSer[i] / factorial[i] * Expval;
			for (int j = 0; j < 50; j++) {
			probj = PowerSer[j] / factorial[j] * Expval;
			for (int k = 0; k < 50; k++) {
				probk = PowerSer[k] / factorial[k] * Expval;
				// joint probability is product of event weights multiplied by weight of input distribution bin
				MC_ints[i][j][k] = MC_ints[i][j][k] + probi * probj * probk * xweight;
			}
			}
		}

    }

    int NDatabin = Data_distr_->GetNbinsX();

    for (int jbin = 1; jbin < NDatabin + 1; jbin++) {
	mean = (Data_distr_->GetBinCenter(jbin)) * scaleFactor;
	xweight = Data_distr_->GetBinContent(jbin);

	// Generate poisson distribution for each value of the mean

	if (mean < 0.) {
	    std::cout << " Your histogram generates Data luminosity values less than zero!" << " Please Check.  Terminating." << std::endl;
	    return;
	}

    Expval = exp(-1. * mean);

	base = 1.;

	for (int i = 1; i < 50; ++i) {
	    base = base * mean;
	    PowerSer[i] = base;
	}

	// compute poisson probability for each Nvtx in weight matrix                                                                  

		for (int i = 0; i < 50; i++)
		{
			probi = PowerSer[i] / factorial[i] * Expval;
			for (int j = 0; j < 50; j++)
			{
				probj = PowerSer[j] / factorial[j] * Expval;
				for (int k = 0; k < 50; k++)
				{
					probk = PowerSer[k] / factorial[k] * Expval;
					// joint probability is product of event weights multiplied by weight of input distribution bin
					Data_ints[i][j][k] = Data_ints[i][j][k] + probi * probj * probk * xweight;
				}
			}
		}
    }

    for (int i = 0; i < 50; i++)
    {
		for (int j = 0; j < 50; j++)
		{
			for (int k = 0; k < 50; k++)
			{
				if ((MC_ints[i][j][k]) > 0.)
					weights3D[i][j][k] = Data_ints[i][j][k] / MC_ints[i][j][k];
				else
					weights3D[i][j][k] = 0.;
			}
		}
    }

	std::cout << weights3D[12][20][5] << "\n";
    return;

}

float PUReweighter::getWeightTruth(float mean)
{
	assert(puDistrData);
	assert(puDistrMC);
	
	float valData = puDistrData->GetBinContent(puDistrData->FindBin(mean));
	float valMC = puDistrMC->GetBinContent(puDistrMC->FindBin(mean));

	if (valMC == 0)
		return 0;

	return valData/valMC;
}

float PUReweighter::getWeightApprox(unsigned int bxM1, unsigned int bx0, unsigned int bxP1)
{
	return weights3D[std::min<unsigned int>(49,bxM1)][std::min<unsigned int>(49,bx0)][std::min<unsigned int>(49,bxP1)];
}

#ifdef WITH_KAPPA
float PUReweighter::getWeight(KGenEventInfo * info)
{
	if (info->nPUMean != -1)
		return getWeightTruth(info);
	else
		return getWeightApprox(info);
}

float PUReweighter::getWeightTruth(KGenEventInfo * info)
{
	return getWeightTruth(info->nPUMean);
}

float PUReweighter::getWeightApprox(KGenEventInfo * info)
{
	return getWeightApprox(info->nPUm1, info->nPU, info->nPUp1);
}
#endif

