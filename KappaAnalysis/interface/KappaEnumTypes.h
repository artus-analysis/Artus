
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

	enum class TauDecayMode : int
	{
		NONE = 5,
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
		LL = 4,
		ET = 5,
		EM = 6,
		MT = 7,
	};
	static DiLeptonDecayMode ToDiLeptonDecayMode(std::string const& diLeptonDecayMode);

	enum class DiLeptonDecayModeLFV : int
	{
		NONE = -1,
		EE = 0,
		MM = 1,
		TT = 2,
		LL = 4,
		ET = 5,
		EM = 6,
		MT = 7,
	};
	static DiLeptonDecayModeLFV ToDiLeptonDecayModeLFV(std::string const& diLeptonDecayModeLFV);

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
	
	enum class ZBosonProductionMode : int
	{
		NONE   = -1,
		D_DBAR = 1,
		U_UBAR = 2,
		S_SBAR = 3,
		C_CBAR = 4,
		B_BBAR = 5
	};

	enum class ValidJetsInput : int
	{
		AUTO = 0,
		UNCORRECTED = 1,
		CORRECTED = 2,
	};
	static ValidJetsInput ToValidJetsInput(std::string const& validJetsInput);

	enum class JetIDVersion : int
	{
		ID2010 = 0,  // old run1 version (most run 1 analyses)
		ID2014 = 1,  // new run1 version (run 1 legacy: old version + muon fraction cut)
		             // first run 2 version identical to run 1 legacy version
		ID73X = 3,   // new run 2 version identical to ID2014 but change in cmssw 7.3.x fraction definitions
		ID73Xtemp = 4, // temporary recommendation for first run 2 events due to ID problems in the forward region
		ID73XnoHF = 5, // as temp but invalidate forward jets
		ID2015 = 6,  // new jet ID for run 2 updated on 2015-09-11
		ID2016 = 7,  // new jet ID for 2016 updated on 2017-03-24
	};
	static JetIDVersion ToJetIDVersion(std::string const& jetIDVersion);

	enum class JetID : int
	{
		NONE,
		LOOSE,
		LOOSELEPVETO,
		MEDIUM,
		TIGHT,
		TIGHTLEPVETO
	};
	static JetID ToJetID(std::string const& jetID);

	enum class BTagScaleFactorMethod : int
	{
		NONE = -1,
		PROMOTIONDEMOTION = 0,
		OTHER = 1,
	};
	static BTagScaleFactorMethod ToBTagScaleFactorMethod(std::string const& bTagSFMethod);

	enum class GenCollectionToPrint : int
	{
		NONE = -1,
		ALL = 0,
		GEN = 1,
		LHE = 2,
	};
	static GenCollectionToPrint ToGenCollectionToPrint(std::string const& genCollectionToPrint);
};

