
#include "Artus/Utility/interface/DefaultValues.h"


typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > RMFLV;
typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > CartesianRMFLV;

const int DefaultValues::UndefinedInt = -999;
const float DefaultValues::UndefinedFloat = -999.0f;
const double DefaultValues::UndefinedDouble = -999.0;
const RMFLV DefaultValues::UndefinedRMFLV = RMFLV(99999.0, 0.0, 0.0, 99999.0);
const CartesianRMFLV DefaultValues::UndefinedCartesianRMFLV = CartesianRMFLV(99999.0, 0.0, 0.0, 99999.0);

const float DefaultValues::EtaBorderEB = 1.479f;

const int DefaultValues::pdgIdGluon = 21;
const int DefaultValues::pdgIdGamma = 22;
const int DefaultValues::pdgIdZ = 23;
const int DefaultValues::pdgIdW = 24;
const int DefaultValues::pdgIdH = 25;
const int DefaultValues::pdgIdHCPEven = 35;
const int DefaultValues::pdgIdACPOdd = 36;

const int DefaultValues::pdgIdPiZero = 111;
const int DefaultValues::pdgIdPiPlus = 211;
const int DefaultValues::pdgIdRhoPlus770 = 213;
const int DefaultValues::pdgIdEta = 221;
const int DefaultValues::pdgIdProton= 2212;

const int DefaultValues::pdgIdKPlus = 321;
const int DefaultValues::pdgIdKStar = 323;
const int DefaultValues::pdgIdKLong = 130;
const int DefaultValues::pdgIdKShort = 310;

const int DefaultValues::pdgIdElectron = 11;
const int DefaultValues::pdgIdNuE = 12;
const int DefaultValues::pdgIdMuon = 13;
const int DefaultValues::pdgIdNuMu = 14;
const int DefaultValues::pdgIdTau = 15;
const int DefaultValues::pdgIdNuTau = 16;

const int DefaultValues::pdgIdAOnePlus1260 = 20213;

const float DefaultValues::ElectronMassGeV = 0.5109989461E-3;
const float DefaultValues::MuonMassGeV = 105.6583745E-3;
const float DefaultValues::TauMassGeV = 1776.86E-3;

const float DefaultValues::ChargedPionMass = 139.57061E-3;
const float DefaultValues::NeutralPionMass = 134.9770E-3;

