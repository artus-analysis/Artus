
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

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

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;

	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE;
 
	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE;
	

private:

};

