
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"

/**
   \brief Place to collect functions of general use
*/
	


class GeneratorInfo {

public:
	static KappaEnumTypes::GenMatchingCode GetGenMatchingCode(KGenParticle* genParticle);
	static RMFLV* GetVisibleLV(KGenParticle* genParticle);
	
	static KGenParticle* GetGenMatchedParticle(
			KLepton* lepton,
			std::map<KLepton*, KGenParticle*> const& leptonGenParticleMap,
			std::map<KLepton*, KGenTau*> const& leptonGenTauMap
	);

	static KappaEnumTypes::GenMatchingCode GetGenMatchingCodeUW(
			KappaTypes::event_type const& event,
			KLepton* lepton
	);
	static std::vector<RMFLV> BuildGenTausUW(
			KappaTypes::event_type const& event
	);

private:
	GeneratorInfo() {  };
};
