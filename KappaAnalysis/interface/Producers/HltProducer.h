
#pragma once

#include <limits>

#include "Kappa/DataFormats/interface/Kappa.h"
#include "KappaTools/RootTools/interface/RunLumiReader.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/Utility/interface/DefaultValues.h"


/** 
 *
 *	
 */
class HltProducer: public KappaProducerBase
{
public:
	void Init(KappaSettings const& settings) override;

	void Produce(KappaEvent const& event, KappaProduct& product,
	             KappaSettings const& settings) const override;

private:
	mutable HLTTools m_hltInfo;

};

