
#pragma once

#include "Artus/Consumer/interface/CutFlowTreeConsumer.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/**
   \brief CutFlow consumer
*/
class KappaCutFlowTreeConsumer: public CutFlowTreeConsumer<KappaTypes> {

public:
	
	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE;
};
