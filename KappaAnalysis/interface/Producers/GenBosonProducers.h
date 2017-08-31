
#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/**
Central producers for searching the resonance boson,
whose final state particles are to be analysed on reco. level.
*/

class GenBosonFromGenParticlesProducer: public ProducerBase<KappaTypes>
{
public:

	std::string GetProducerId() const override;

	void Init(setting_type const& settings, metadata_type& metadata) override;

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override;

protected:

	void FindGenBoson(event_type const& event, product_type& product,
	                  setting_type const& settings, unsigned int startIndex=0) const;
};


class GenBosonProductionProducer: public GenBosonFromGenParticlesProducer
{
public:

	std::string GetProducerId() const override;

	void Init(setting_type const& settings, metadata_type& metadata) override;

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override;

protected:

	std::vector<KGenParticle*> FindMothersWithDifferentPdgId(KGenParticles* genParticles, unsigned int currentIndex, int currentPdgId) const;
};


class GenBosonDiLeptonDecayModeProducer: public GenBosonFromGenParticlesProducer
{
public:

	std::string GetProducerId() const override;

	void Init(setting_type const& settings, metadata_type& metadata) override;

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override;

protected:

	void FindGenDiLeptons(event_type const& event, product_type& product,
	                      setting_type const& settings) const;
};


class GenBosonDiLeptonDecayModeLFVProducer: public GenBosonFromGenParticlesProducer
{
public:

	std::string GetProducerId() const override;

	void Init(setting_type const& settings, metadata_type& metadata) override;

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override;

protected:

	void FindGenDiLeptons(event_type const& event, product_type& product,
	                      setting_type const& settings) const;
};
