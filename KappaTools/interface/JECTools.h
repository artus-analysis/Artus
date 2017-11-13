/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#ifdef USE_JEC
#ifndef KAPPA_JECTOOLS_H
#define KAPPA_JECTOOLS_H

#include <CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h>
#include <CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h>
#include <CondFormats/JetMETObjects/interface/JetCorrectorParameters.h>
#include <Kappa/DataFormats/interface/Kappa.h>
#include "Artus/KappaTools/interface/SortTools.h"
#include "Artus/KappaTools/interface/IOHelper.h"
#include "Artus/KappaTools/interface/Math.h"

enum JECValueType { jec_center, jec_up, jec_down };

// Function to setup the FactorizedJetCorrector / JetCorrectionUncertainty object

template<typename Tprov, typename Tjet>
inline void setupFactorProvider(const Tjet &jet, Tprov *prov)
{
	prov->setJetEta(jet.p4.eta());
	prov->setJetPt(jet.p4.pt());
	prov->setJetE(jet.p4.E());
	prov->setJetPhi(jet.p4.phi());
}

// Function to correct a single jet with the FactorizedJetCorrector

template<typename T>
inline void correctSingleJet(T &jet, FactorizedJetCorrector *jec)
{
	setupFactorProvider(jet, jec);
	jec->setJetA(jet.area);
	jet.p4 *= jec->getCorrection();
}

// Functions to apply correction + uncertainty to a single jet:

template<typename T>
inline void applyUncertainty(T &jet, JetCorrectionUncertainty *unc, float shift = 0.0f)
{
	if ((unc != nullptr) && (std::abs(shift) > 0.00001f))
	{
		setupFactorProvider(jet, unc);
		jet.p4 *= static_cast<float>(1.0f + (shift * unc->getUncertainty(shift > 0.0f)));
	}
}

template<typename T>
inline void applyUncertainty(T &jet, JetCorrectionUncertainty *unc, const JECValueType jv = jec_center)
{
	float shift = 0.0;
	if (jv == jec_up)
		shift = 1.0f;
	else if (jv == jec_down)
		shift = -1.0f;

	applyUncertainty(jet, unc, shift);
}

template<typename T>
inline void correctJets(std::vector<T> *jets,
	FactorizedJetCorrector *jec, JetCorrectionUncertainty *unc,
	const double rho, const int npv, const float area = -1, float shift = 0.0, bool sort = true)
{
	for (size_t idx = 0; idx < jets->size(); ++idx)
	{
		T &jet = jets->at(idx);
		if (std::abs(jet.p4.Eta()) < 5.4f)
		{
			if (area > 0)
			{
				jet.area = area;
			}
			if (jec != nullptr)
			{
				jec->setRho(static_cast<float>(rho));
				jec->setNPV(npv);
				correctSingleJet(jet, jec);
			}
			if (unc != nullptr)
			{
				applyUncertainty(jet, unc, shift);
			}
		}
	}
	if (sort)
	{
		sort_pt(jets);
	}
}

template<typename T>
inline void correctJets(std::vector<T> *jets,
	FactorizedJetCorrector *jec, JetCorrectionUncertainty *unc,
	const double rho, const int npv, const float area = -1, const JECValueType jv = jec_center, bool sort = true)
{
	float shift = 0.0;
	if (jv == jec_up)
		shift = 1.0f;
	else if (jv == jec_down)
		shift = -1.0f;

	correctJets(jets, jec, unc, rho, npv, area, shift, sort);
}

#include "FileInterfaceBase.h"

class JECService
{
public:
	JECService(FileInterfaceBase &fi, const std::string prefix, const std::vector<std::string> &level, const double R, const int jeuDir = 0, std::string const &pu_density_branch = "KT6Area")
		: area(M_PI * sqr(R)), jeuType(jec_center), JEC(nullptr), JEU(nullptr),
			vs(fi.GetEvent<KVertexSummary>("offlinePrimaryVerticesSummary", false)),
			ja(fi.GetEvent<KPileupDensity>(pu_density_branch, true, true))
	{
		if (R < 0)
			area = -1;
		std::cout << yellow << " * Loading jet energy corrections..." << reset << std::endl << "\t";
		std::vector<JetCorrectorParameters> jecVec;
		for (size_t i = 0; i < level.size(); ++i)
		{
			std::cout << level[i] << " ";
			jecVec.push_back(JetCorrectorParameters(prefix + level[i] + ".txt"));
		}
		JEC = new FactorizedJetCorrector(jecVec);
		std::cout << std::endl;

		std::cout << yellow << " * Loading jet energy uncertainties..." << reset << std::endl;
		JEU = new JetCorrectionUncertainty(prefix + "Uncertainty.txt");
		if (jeuDir > 0)
			jeuType = jec_up;
		else if (jeuDir < 0)
			jeuType = jec_down;
		else
			jeuType = jec_center;
	}

	~JECService()
	{
		delete JEU;
		delete JEC;
	}

	template<typename T>
	inline void correct(T *jets)
	{
		correctJets(jets, JEC, JEU, ja->rho, vs->nVertices, area, jeuType);
	}

private:
	double area;
	JECValueType jeuType;
	FactorizedJetCorrector *JEC;
	JetCorrectionUncertainty *JEU;
	KVertexSummary *vs;
	KPileupDensity *ja;
};

#endif
#endif
