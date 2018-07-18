
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"


KappaEnumTypes::GenParticleType KappaEnumTypes::ToGenParticleType(std::string const& genParticleName)
{
	if (genParticleName == "genParticle") return KappaEnumTypes::GenParticleType::GENPARTICLE;
	else if (genParticleName == "genElectron") return KappaEnumTypes::GenParticleType::GENELECTRON;
	else if (genParticleName == "genMuon") return KappaEnumTypes::GenParticleType::GENMUON;
	else if (genParticleName == "genTau") return KappaEnumTypes::GenParticleType::GENTAU;
	else return KappaEnumTypes::GenParticleType::NONE;
}

KappaEnumTypes::DiLeptonDecayMode KappaEnumTypes::ToDiLeptonDecayMode(std::string const& diLeptonDecayMode)
{
	if (diLeptonDecayMode == "ee") return KappaEnumTypes::DiLeptonDecayMode::EE;
	else if (diLeptonDecayMode == "mm") return KappaEnumTypes::DiLeptonDecayMode::MM;
	else if (diLeptonDecayMode == "tt") return KappaEnumTypes::DiLeptonDecayMode::TT;
	else if (diLeptonDecayMode == "ll") return KappaEnumTypes::DiLeptonDecayMode::LL;
	else if (diLeptonDecayMode == "em") return KappaEnumTypes::DiLeptonDecayMode::EM;
	else if (diLeptonDecayMode == "et") return KappaEnumTypes::DiLeptonDecayMode::ET;
	else if (diLeptonDecayMode == "mt") return KappaEnumTypes::DiLeptonDecayMode::MT;
	return KappaEnumTypes::DiLeptonDecayMode::NONE;
}

KappaEnumTypes::DiLeptonDecayModeLFV KappaEnumTypes::ToDiLeptonDecayModeLFV(std::string const& diLeptonDecayModeLFV)
{
	if (diLeptonDecayModeLFV == "ee") return KappaEnumTypes::DiLeptonDecayModeLFV::EE;
	else if (diLeptonDecayModeLFV == "mm") return KappaEnumTypes::DiLeptonDecayModeLFV::MM;
	else if (diLeptonDecayModeLFV == "tt") return KappaEnumTypes::DiLeptonDecayModeLFV::TT;
	else if (diLeptonDecayModeLFV == "ll") return KappaEnumTypes::DiLeptonDecayModeLFV::LL;
	else if (diLeptonDecayModeLFV == "em") return KappaEnumTypes::DiLeptonDecayModeLFV::EM;
	else if (diLeptonDecayModeLFV == "et") return KappaEnumTypes::DiLeptonDecayModeLFV::ET;
	else if (diLeptonDecayModeLFV == "mt") return KappaEnumTypes::DiLeptonDecayModeLFV::MT;
	return KappaEnumTypes::DiLeptonDecayModeLFV::NONE;
}

KappaEnumTypes::ValidJetsInput KappaEnumTypes::ToValidJetsInput(std::string const& validJetsInput)
{
	if (validJetsInput == "uncorrected") return KappaEnumTypes::ValidJetsInput::UNCORRECTED;
	else if (validJetsInput == "corrected") return KappaEnumTypes::ValidJetsInput::CORRECTED;
	else return KappaEnumTypes::ValidJetsInput::AUTO;
}

KappaEnumTypes::JetIDVersion KappaEnumTypes::ToJetIDVersion(std::string const& jetIDVersion)
{
	if (jetIDVersion == "2010") return KappaEnumTypes::JetIDVersion::ID2010;
	else if (jetIDVersion == "2014") return KappaEnumTypes::JetIDVersion::ID2014;
	else if (jetIDVersion == "73x") return KappaEnumTypes::JetIDVersion::ID73X;
	else if (jetIDVersion == "73xtemp") return KappaEnumTypes::JetIDVersion::ID73Xtemp;
	else if (jetIDVersion == "73xnohf") return KappaEnumTypes::JetIDVersion::ID73XnoHF;
	else if (jetIDVersion == "2015") return KappaEnumTypes::JetIDVersion::ID2015;
	else if (jetIDVersion == "2016") return KappaEnumTypes::JetIDVersion::ID2016;
	else if (jetIDVersion == "2017") return KappaEnumTypes::JetIDVersion::ID2017;
	else LOG(FATAL) << "Jet ID version '" << jetIDVersion << "' is not available";
	return KappaEnumTypes::JetIDVersion::ID2016;
}

KappaEnumTypes::JetID KappaEnumTypes::ToJetID(std::string const& jetID)
{
	if (jetID == "loose") return KappaEnumTypes::JetID::LOOSE;
	else if (jetID == "looselepveto") return KappaEnumTypes::JetID::LOOSELEPVETO;
	else if (jetID == "medium") return KappaEnumTypes::JetID::MEDIUM;
	else if (jetID == "tight") return KappaEnumTypes::JetID::TIGHT;
	else if (jetID == "tightlepveto") return KappaEnumTypes::JetID::TIGHTLEPVETO;
	else if (jetID == "none") return KappaEnumTypes::JetID::NONE;
	else LOG(FATAL) << "Jet ID of type '" << jetID << "' not implemented!";
	return KappaEnumTypes::JetID::NONE;
}

KappaEnumTypes::BTagScaleFactorMethod KappaEnumTypes::ToBTagScaleFactorMethod(std::string const& bTagSFMethod)
{
	if (bTagSFMethod == "promotiondemotion") return KappaEnumTypes::BTagScaleFactorMethod::PROMOTIONDEMOTION;
	else if (bTagSFMethod == "other") return KappaEnumTypes::BTagScaleFactorMethod::OTHER;
	else return KappaEnumTypes::BTagScaleFactorMethod::NONE;
}

KappaEnumTypes::GenCollectionToPrint KappaEnumTypes::ToGenCollectionToPrint(std::string const& genCollectionToPrint)
{
	if (genCollectionToPrint == "all") return KappaEnumTypes::GenCollectionToPrint::ALL;
	else if (genCollectionToPrint == "gen") return KappaEnumTypes::GenCollectionToPrint::GEN;
	else if (genCollectionToPrint == "lhe") return KappaEnumTypes::GenCollectionToPrint::LHE;
	else return KappaEnumTypes::GenCollectionToPrint::NONE;
}

