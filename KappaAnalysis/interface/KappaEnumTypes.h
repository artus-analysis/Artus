
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

	enum class GenMatchingCode : int
	{
		NONE             = -1,
		IS_ELE_PROMPT    = 1,
		IS_MUON_PROMPT   = 2,
		IS_ELE_FROM_TAU  = 3,
		IS_MUON_FROM_TAU = 4,
		IS_TAU_HAD_DECAY = 5,
		IS_FAKE          = 6
	};

};

