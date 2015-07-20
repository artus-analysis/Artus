
#pragma once

#include <limits>

#include "Kappa/DataFormats/interface/Kappa.h"
#include "KappaTools/RootTools/RunLumiReader.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/Utility/interface/DefaultValues.h"


/** 
 *
 *	
 */
class HltProducer: public KappaProducerBase
{
public:
	virtual std::string GetProducerId() const override;

	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;

};

