#pragma once

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"
#include "Artus/KappaAnalysis/interface/Utility/GenParticleDecayTreePrinter.h"


class PrintGenParticleDecayTreeProducer : public KappaProducerBase {
public:
	std::string GetProducerId() const override;
	void Init(setting_type const& settings, metadata_type& metadata) override;
	void Produce(event_type const& event, product_type& product, setting_type const& settings, metadata_type const& metadata) const override;

private:
	GenParticleDecayTreePrinter genParticleDecayTreePrinter;
	KappaEnumTypes::GenCollectionToPrint genCollectionToPrint = KappaEnumTypes::GenCollectionToPrint::NONE;

};
