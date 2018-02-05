#pragma once

#include "Artus/Utility/interface/Utility.h"

// a set of default values that can be used in your physics analysis
// be aware. Comparing 2 floating point values like ( a == b ) is 
// never a good idea and the outcome can depend of compile and platform
class DefaultValues {
public:
	typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > RMFLV;
	typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > CartesianRMFLV;

	// values set in .cc file
	static const int UndefinedInt;
	static const float UndefinedFloat;
	static const double UndefinedDouble;
	static const RMFLV UndefinedRMFLV;
	static const CartesianRMFLV UndefinedCartesianRMFLV;
	
	static const float EtaBorderEB;
	
	static const int pdgIdGluon;
	static const int pdgIdGamma;
	static const int pdgIdZ;
	static const int pdgIdW;
	static const int pdgIdH;
	static const int pdgIdHCPEven;
	static const int pdgIdACPOdd;

	static const int pdgIdPiZero;
	static const int pdgIdPiPlus;
	static const int pdgIdRhoPlus770;
	static const int pdgIdEta;
	static const int pdgIdProton;

	static const int pdgIdKPlus;
	static const int pdgIdKStar;
	static const int pdgIdKLong;
	static const int pdgIdKShort;

	static const int pdgIdElectron;
	static const int pdgIdNuE;
	static const int pdgIdMuon;
	static const int pdgIdNuMu;
	static const int pdgIdTau;
	static const int pdgIdNuTau;
	
	static const int pdgIdDown;
	static const int pdgIdUp;
	static const int pdgIdStrange;
	static const int pdgIdCharm;
	static const int pdgIdBottom;
	static const int pdgIdTop;

	static const int pdgIdAOnePlus1260;

	static const float ElectronMassGeV;
	static const float MuonMassGeV;
	static const float TauMassGeV;
	
	static const float ChargedPionMass;
	static const float NeutralPionMass;
	
	static const float WBosonMassGeV;
	static const float ZBosonMassGeV;
};
