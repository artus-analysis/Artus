
#pragma once

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/KappaAnalysis/interface/Utility/ValidPhysicsObjectTools.h"


/**
Central producers for collecting valid gen. particles originating from boson decays
Needs GenDiLeptonDecayModeProducer to be run before this producers.
*/

class ValidGenParticlesProducer: public ProducerBase<KappaTypes>, public ValidPhysicsObjectTools<KappaTypes, KGenParticle>
{
public:

	ValidGenParticlesProducer(std::vector<KGenParticle*> KappaTypes::product_type::*genParticles,
	                          int absPdgId,
	                          std::vector<KGenParticle*> KappaTypes::product_type::*validLeptons,
	                          std::vector<std::string>& (KappaTypes::setting_type::*GetLowerPtCuts)(void) const,
	                          std::vector<std::string>& (KappaTypes::setting_type::*GetUpperAbsEtaCuts)(void) const,
	                          std::string name);

	void Init(KappaTypes::setting_type const& settings, KappaTypes::metadata_type& metadata) override;

	void Produce(KappaTypes::event_type const& event, KappaTypes::product_type& product,
	             KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) const override;

protected:
	std::vector<KGenParticle*> KappaTypes::product_type::*m_validLeptonsMember;
	std::string m_name;

private:
	std::vector<KGenParticle*> KappaTypes::product_type::*m_genParticlesMember;
	int m_absPdgId;
	
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(KGenParticle* genParticle, KappaTypes::event_type const& event, KappaTypes::product_type& product,
	                                KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) const;
};


class ValidGenElectronsProducer: public ValidGenParticlesProducer
{
public:

	ValidGenElectronsProducer();

	std::string GetProducerId() const override;

	void Init(KappaTypes::setting_type const& settings, KappaTypes::metadata_type& metadata) override;
};


class ValidGenMuonsProducer: public ValidGenParticlesProducer
{
public:

	ValidGenMuonsProducer();

	std::string GetProducerId() const override;

	void Init(KappaTypes::setting_type const& settings, KappaTypes::metadata_type& metadata) override;
};


class ValidGenTausProducer: public ValidGenParticlesProducer
{
public:

	ValidGenTausProducer();

	std::string GetProducerId() const override;

	void Init(KappaTypes::setting_type const& settings, KappaTypes::metadata_type& metadata) override;
	
	void Produce(KappaTypes::event_type const& event, KappaTypes::product_type& product,
	             KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata) const override;

private:
	std::pair<std::vector<KGenParticle*>, std::vector<KGenParticle*> > GetChargedNeutralHadrons(KGenParticles* genParticles, KGenParticle* genTau) const;
};

