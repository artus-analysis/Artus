
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/KappaAnalysis/interface/Consumers/KappaLambdaNtupleConsumer.h"


/**
   \brief GlobalProducer to write out genLevel muon FSR information to matched muons

   Run after ValidMuonProducer and GenParticleProducer
   Steps of the producer
   1. genMatching to find genMuons that match a reconstructed muon
   2. find the FSR emitted by the genMuon
   3. write out the genMuonFSR to a new quantity genMuonFSR
*/

class GenMuonFSRProducer: public KappaProducerBase
{
public:

	std::string GetProducerId() const override;

	void Init(setting_type const& settings, metadata_type& metadata) override;

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override;

private:

};

