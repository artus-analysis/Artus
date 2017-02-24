
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/**
   \brief Producer for tau four momentum corrections.

   (No correction implemented yet.)
*/
class TauCorrectionsProducer: public KappaProducerBase
{

public:
	void Init(KappaSettings const& settings)  override;

	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;


protected:
	// Can be overwritten for analysis-specific use cases
	virtual void AdditionalCorrections(KTau* tau, KappaEvent const& event,
	                                   KappaProduct& product, KappaSettings const& settings) const;

};

