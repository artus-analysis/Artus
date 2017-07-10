#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/GenParticleDecayTreePrinter.h"


class PrintGenParticleDecayTreeProducer : public KappaProducerBase {
public:
	std::string GetProducerId() const override;
	void Init(KappaSettings const& settings) override;
	void Produce(KappaEvent const& event, KappaProduct & product, KappaSettings const& settings) const override;

private:
	GenParticleDecayTreePrinter genParticleDecayTreePrinter;

};
