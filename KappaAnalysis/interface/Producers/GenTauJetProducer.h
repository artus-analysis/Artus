
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"

/**
   \brief GlobalProducer, to write any GenTauJet to the product.
   
   genTauJets are objects built from the CMSSW collections named "TauGenJets"
   and "TauGenJetsDecayModeSelectorAllHadrons" (the last one being a subset of 
   the first, after a selection on the decay mode). They are constructed from
   the stable visible decay products of taus. 

   The producer fills them in a 
*/

class GenTauJetProducer: public KappaProducerBase
{
public:

	std::string GetProducerId() const override;

	void Init(setting_type const& settings, metadata_type& metadata) override;
 
	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override;

};

