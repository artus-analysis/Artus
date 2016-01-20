
#pragma once

#include "Artus/KappaAnalysis/interface/KappaSettings.h"


class KappaExampleSettings: public KappaSettings {
public:

	IMPL_SETTING(float, DiMuonMassLowerCut);
	IMPL_SETTING(bool, OSMuons);
};
