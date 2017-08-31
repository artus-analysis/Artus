#pragma once

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
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

	void Init(setting_type const& settings, metadata_type& metadata) override;
	
	std::string GetProducerId() const override
	{
		return "PFCandidatesProducer";
	};
	
	void Produce(event_type const& event, product_type& product, setting_type const& settings, metadata_type const& metadata) const override;

private:
	void fill_pfCandidate(std::vector<const KPFCandidate*>&, std::vector<const KPFCandidate*>&, std::vector<const KPFCandidate*>&, const KPFCandidate*) const;
};
