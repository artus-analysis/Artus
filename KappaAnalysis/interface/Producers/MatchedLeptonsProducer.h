#pragma once

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/** Producer that collects valid electrons-,muons-,taus-map(reco-gen) together as a leptons-map(reco-gen)
 *
 *  Should run after the last change to the maps of electrons/muons/taus-MatchingProducer 
 */
class MatchedLeptonsProducer: public KappaProducerBase
{

public:
	std::string GetProducerId() const override;

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override;

};

