
#pragma once

#include "Artus/Utility/interface/ArtusLogging.h"


/**
*/
class KappaEnumTypes {

public:

	enum class GenParticleType : int
	{
		NONE = -1,
		GENPARTICLE  = 0,
		GENELECTRON = 1,
		GENMUON = 2,
		GENTAU = 3
	};
	static GenParticleType ToGenParticleType(std::string const& genParticleName);
	
	enum class TauTauDecayMode : int
	{
		NONE = -1,
		TT   = 1,
		MT   = 2,
		ET   = 3,
		MM   = 4,
		EM   = 5,
		EE   = 6
	};

	enum class DiLeptonDecayMode : int
	{
		NONE = -1,
		EE = 0,
		MM = 1,
		TT = 2,
		LL = 3,
	};
	static DiLeptonDecayMode ToDiLeptonDecayMode(std::string const& diLeptonDecayMode);

};

