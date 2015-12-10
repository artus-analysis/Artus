
#pragma once

#include "Artus/Consumer/interface/CutFlowTreeConsumer.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/**
   \brief CutFlow consumer
*/
class KappaCutFlowTreeConsumer: public CutFlowTreeConsumer<KappaTypes> {

public:
	
	void Init(KappaSettings const& settings) override;
};
