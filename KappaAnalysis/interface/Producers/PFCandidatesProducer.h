#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"

/**
   \brief Producer, that devides packedPFCandidates according to pddId. Source :
   https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD2016

   Several simple quantities are provided like number of PFCandidates of certain flavour,
   vector and absolute sum over Pt of all Candidates.
*/

class PFCandidatesProducer : public KappaProducerBase
{
    public:

        void Init(KappaSettings const& settings) override;
	std::string GetProducerId() const override { return "PFCandidatesProducer"; };
        void Produce(KappaEvent const& event, KappaProduct& product, KappaSettings const& settings) const override;
};