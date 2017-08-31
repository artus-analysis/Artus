
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/ValidPhysicsObjectTools.h"


class ValidGenJetsProducer: public KappaProducerBase, public ValidPhysicsObjectTools<KappaTypes, KGenJet>
{

public:

	ValidGenJetsProducer();
	
	virtual std::string GetProducerId() const override;
	virtual void Init(KappaTypes::setting_type const& settings, KappaTypes::metadata_type& metadata) override;
	virtual void Produce(KappaTypes::event_type const& event, KappaTypes::product_type& product,
	                     KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) const override;

protected:
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KGenJet* jet, KappaTypes::event_type const& event, KappaTypes::product_type& product,
	                                KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) const;

};

