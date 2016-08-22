
#include "Artus/KappaAnalysis/interface/Producers/ValidGenParticlesProducers.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/Utility/interface/DefaultValues.h"


ValidGenParticlesProducer::ValidGenParticlesProducer(std::vector<KGenParticle*> product_type::*genParticles,
                                                     int pdgId,
                                                     std::vector<KGenParticle*> product_type::*validLeptons,
                                                     std::vector<std::string>& (setting_type::*GetLowerPtCuts)(void) const,
                                                     std::vector<std::string>& (setting_type::*GetUpperAbsEtaCuts)(void) const) :
	ProducerBase<KappaTypes>(),
	ValidPhysicsObjectTools<KappaTypes, KGenParticle>(GetLowerPtCuts, GetUpperAbsEtaCuts, validLeptons),
	m_genParticlesMember(genParticles),
	m_pdgId(pdgId),
	m_validLeptonsMember(validLeptons)
{
}

void ValidGenParticlesProducer::Init(KappaSettings const& settings)
{
	ProducerBase<KappaTypes>::Init(settings);
}

void ValidGenParticlesProducer::Produce(event_type const& event, product_type& product, KappaSettings const& settings) const
{
	for (std::vector<KGenParticle*>::iterator genParticle = (product.*m_genParticlesMember).begin();
	     genParticle != (product.*m_genParticlesMember).end(); ++genParticle)
	{
		bool validLepton = ((*genParticle)->pdgId == m_pdgId);
		
		// kinematic cuts
		validLepton = validLepton && this->PassKinematicCuts(*genParticle, event, product);
	
		// check possible analysis-specific criteria
		validLepton = validLepton && AdditionalCriteria(*genParticle, event, product, settings);
		
		if (validLepton)
		{
			 (product.*m_validLeptonsMember).push_back(*genParticle);
		}
	}
}

bool ValidGenParticlesProducer::AdditionalCriteria(KGenParticle* genParticle, event_type const& event,
                                product_type& product, setting_type const& settings) const
{
	bool validLepton = true;
	return validLepton;
}


ValidGenElectronsProducer::ValidGenElectronsProducer() :
	ValidGenParticlesProducer(&product_type::m_genLeptonsFromBosonDecay,
	                          DefaultValues::pdgIdElectron,
	                          &product_type::m_validGenElectrons,
	                          &setting_type::GetGenElectronLowerPtCuts,
	                          &setting_type::GetGenElectronUpperAbsEtaCuts)
{
}

std::string ValidGenElectronsProducer::GetProducerId() const {
	return "ValidGenElectronsProducer";
}

void ValidGenElectronsProducer::Init(KappaSettings const& settings)
{
	ValidGenParticlesProducer::Init(settings);
}


ValidGenMuonsProducer::ValidGenMuonsProducer() :
	ValidGenParticlesProducer(&product_type::m_genLeptonsFromBosonDecay,
	                          DefaultValues::pdgIdMuon,
	                          &product_type::m_validGenMuons,
	                          &setting_type::GetGenMuonLowerPtCuts,
	                          &setting_type::GetGenMuonUpperAbsEtaCuts)
{
}

std::string ValidGenMuonsProducer::GetProducerId() const {
	return "ValidGenMuonsProducer";
}

void ValidGenMuonsProducer::Init(KappaSettings const& settings)
{
	ValidGenParticlesProducer::Init(settings);
}


ValidGenTausProducer::ValidGenTausProducer() :
	ValidGenParticlesProducer(&product_type::m_genLeptonsFromBosonDecay,
	                          DefaultValues::pdgIdTau,
	                          &product_type::m_validGenTaus,
	                          &setting_type::GetGenTauLowerPtCuts,
	                          &setting_type::GetGenTauUpperAbsEtaCuts)
{
}

std::string ValidGenTausProducer::GetProducerId() const {
	return "ValidGenTausProducer";
}

void ValidGenTausProducer::Init(KappaSettings const& settings)
{
	ValidGenParticlesProducer::Init(settings);
}

