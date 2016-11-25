#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"

/**
   \brief Producer, that devides packedPFCandidates according to pddId. Source :
   https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD2016
*/

class PFCandidatesProducer : public KappaProducerBase
{
	public:
		void Init(KappaSettings const& settings) override;
		void Produce(KappaEvent const& event, KappaProduct& product, KappaSettings const& settings) const override;
	
	private:
		void fill_pfCandidate(std::vector<const KPFCandidate*>&, std::vector<const KPFCandidate*>&, std::vector<const KPFCandidate*>&, const KPFCandidate*) const;
};
