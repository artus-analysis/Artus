
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/**
   \brief Producer for electron four momentum corrections.
   
   (No correction implemented yet.)
*/
class ElectronCorrectionsProducer: public KappaProducerBase
{

public:

	std::string GetProducerId() const override;
	
	void Init(setting_type const& settings, metadata_type& metadata)  override;

	void Produce(event_type const& event, product_type & product,
	             setting_type const& settings, metadata_type const& metadata) const override;


protected:

	// Can be overwritten for analysis-specific use cases
	virtual void AdditionalCorrections(KElectron* electron, event_type const& event,
	                                   product_type& product, setting_type const& settings, metadata_type const& metadata) const;

};

