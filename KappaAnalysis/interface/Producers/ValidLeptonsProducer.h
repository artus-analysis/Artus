
#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/** Producer that collects (in-) valid electrons/muons/taus together as leptons
 *
 *  Should run after the last change to the vectors of (in-) valid electrons/muons/taus
 */
class ValidLeptonsProducer: public KappaProducerBase
{

public:
	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;

};

