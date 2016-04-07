
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
	return KappaEnumTypes::DiLeptonDecayMode::NONE;
}

