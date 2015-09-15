
#pragma once

#include "Artus/Consumer/interface/CutFlowHistogramConsumer.h"
#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/**
   \brief CutFlow consumer
   Config tags:
   - EventWeight, e.g. "eventWeight"
   
   Writes out cutflow histograms, one non-weighted and one weighted.
   The weight is taken from product.m_weights[settings.GetEventWeight()].
   If you wish a custom weight, derive from this (or its upper) class and
   fill the memember weightExtractor according to you requirements.
*/
class KappaCutFlowHistogramConsumer: public CutFlowHistogramConsumer<KappaTypes> {

public:
	
	virtual void Init(KappaSettings const& settings) override;
};
