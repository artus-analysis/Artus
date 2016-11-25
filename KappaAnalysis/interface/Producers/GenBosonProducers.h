
#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/**
Central producers for searching the resonance boson,
whose final state particles are to be analysed on reco. level.
*/

class GenBosonFromGenParticlesProducer: public ProducerBase<KappaTypes>
{
public:
	void Init(KappaSettings const& settings) override;

	void Produce(KappaEvent const& event, KappaProduct& product,
	             KappaSettings const& settings) const override;
};


class GenBosonDiLeptonDecayModeProducer: public GenBosonFromGenParticlesProducer
{
public:
	void Init(KappaSettings const& settings) override;

	void Produce(KappaEvent const& event, KappaProduct& product,
	             KappaSettings const& settings) const override;
};

