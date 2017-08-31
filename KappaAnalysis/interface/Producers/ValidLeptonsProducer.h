
#pragma once

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/** Producer that collects (in-) valid electrons/muons/taus together as leptons
 *
 *  Should run after the last change to the vectors of (in-) valid electrons/muons/taus
 */
class ValidLeptonsProducer: public KappaProducerBase
{

public:
	std::string GetProducerId() const override;

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override;

};

