
#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"


/**
Central producers for searching the resonance boson,
whose final state particles are to be analysed on reco. level.
*/

class GenBosonFromGenParticlesProducer: public ProducerBase<KappaTypes>
{
public:

	std::string GetProducerId() const override;

	void Init(KappaSettings const& settings) override;

	void Produce(KappaEvent const& event, KappaProduct& product,
	             KappaSettings const& settings) const override;

protected:

	void FindGenBoson(KappaEvent const& event, KappaProduct& product,
	                  KappaSettings const& settings, unsigned int startIndex=0) const;
};


class GenBosonProductionProducer: public GenBosonFromGenParticlesProducer
{
public:

	std::string GetProducerId() const override;

	void Init(KappaSettings const& settings) override;

	void Produce(KappaEvent const& event, KappaProduct& product,
	             KappaSettings const& settings) const override;

protected:

	std::vector<KGenParticle*> FindMothersWithDifferentPdgId(KGenParticles* genParticles, unsigned int currentIndex, int currentPdgId) const;
};


class GenBosonDiLeptonDecayModeProducer: public GenBosonFromGenParticlesProducer
{
public:

	std::string GetProducerId() const override;

	void Init(KappaSettings const& settings) override;

	void Produce(KappaEvent const& event, KappaProduct& product,
	             KappaSettings const& settings) const override;

protected:

	void FindGenDiLeptons(KappaEvent const& event, KappaProduct& product,
	                      KappaSettings const& settings) const;
};

