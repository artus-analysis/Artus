
#pragma once

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/ValidPhysicsObjectTools.h"


/**
Central producers for collecting valid gen. particles originating from boson decays
Needs GenDiLeptonDecayModeProducer to be run before this producers.
*/

class ValidGenParticlesProducer: public ProducerBase<KappaTypes>, public ValidPhysicsObjectTools<KappaTypes, KGenParticle>
{
public:

	typedef typename KappaTypes::event_type event_type;
	typedef typename KappaTypes::product_type product_type;
	typedef typename KappaTypes::setting_type setting_type;
	
	ValidGenParticlesProducer(std::vector<KGenParticle*> product_type::*genParticles,
	                          int pdgId,
	                          std::vector<KGenParticle*> product_type::*validLeptons,
	                          std::vector<std::string>& (setting_type::*GetLowerPtCuts)(void) const,
	                          std::vector<std::string>& (setting_type::*GetUpperAbsEtaCuts)(void) const);

	void Init(setting_type const& settings) override;

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings) const override;

private:
	std::vector<KGenParticle*> product_type::*m_genParticlesMember;
	int m_pdgId;
	std::vector<KGenParticle*> product_type::*m_validLeptonsMember;
	
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KGenParticle* genParticle, event_type const& event,
	                                product_type& product, setting_type const& settings) const;
};


class ValidGenElectronsProducer: public ValidGenParticlesProducer
{
public:

	typedef typename KappaTypes::event_type event_type;
	typedef typename KappaTypes::product_type product_type;
	typedef typename KappaTypes::setting_type setting_type;
	
	ValidGenElectronsProducer();

	std::string GetProducerId() const override;

	void Init(setting_type const& settings) override;
};


class ValidGenMuonsProducer: public ValidGenParticlesProducer
{
public:

	typedef typename KappaTypes::event_type event_type;
	typedef typename KappaTypes::product_type product_type;
	typedef typename KappaTypes::setting_type setting_type;
	
	ValidGenMuonsProducer();

	std::string GetProducerId() const override;

	void Init(setting_type const& settings) override;
};


class ValidGenTausProducer: public ValidGenParticlesProducer
{
public:

	typedef typename KappaTypes::event_type event_type;
	typedef typename KappaTypes::product_type product_type;
	typedef typename KappaTypes::setting_type setting_type;
	
	ValidGenTausProducer();

	std::string GetProducerId() const override;

	void Init(setting_type const& settings) override;
};

