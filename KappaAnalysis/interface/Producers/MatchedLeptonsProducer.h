#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/** Producer that collects valid electrons-,muons-,taus-map(reco-gen) together as a leptons-map(reco-gen)
 *
 *  Should run after the last change to the maps of electrons/muons/taus-MatchingProducer 
 */
class MatchedLeptonsProducer: public KappaProducerBase
{

public:
	std::string GetProducerId() const;
	
	void Produce(KappaEvent const& event, KappaProduct& product,
	             KappaSettings const& settings) const override;

};

