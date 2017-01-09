
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/ValidPhysicsObjectTools.h"
#include "Artus/KappaAnalysis/interface/KappaProduct.h"


class ValidGenJetsProducer: public KappaProducerBase, public ValidPhysicsObjectTools<KappaTypes, KGenJet>
{

public:

	ValidGenJetsProducer();
	
	virtual std::string GetProducerId() const override;
	virtual void Init(KappaSettings const& settings) override;
	virtual void Produce(KappaEvent const& event, KappaProduct& product, KappaSettings const& settings) const override;

protected:
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KGenJet* jet, KappaEvent const& event,
	                                KappaProduct& product, KappaSettings const& settings) const;

};

