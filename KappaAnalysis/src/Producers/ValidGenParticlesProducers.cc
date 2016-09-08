
#include "Artus/KappaAnalysis/interface/Producers/ValidGenParticlesProducers.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"

#include <limits>

#include <Math/VectorUtil.h>


ValidGenParticlesProducer::ValidGenParticlesProducer(std::vector<KGenParticle*> product_type::*genParticles,
                                                     int absPdgId,
                                                     std::vector<KGenParticle*> product_type::*validLeptons,
                                                     std::vector<std::string>& (setting_type::*GetLowerPtCuts)(void) const,
                                                     std::vector<std::string>& (setting_type::*GetUpperAbsEtaCuts)(void) const) :
	ProducerBase<KappaTypes>(),
	ValidPhysicsObjectTools<KappaTypes, KGenParticle>(GetLowerPtCuts, GetUpperAbsEtaCuts, validLeptons),
	m_validLeptonsMember(validLeptons),
	m_genParticlesMember(genParticles),
	m_absPdgId(absPdgId)
{
}

void ValidGenParticlesProducer::Init(KappaSettings const& settings)
{
	ProducerBase<KappaTypes>::Init(settings);
}

void ValidGenParticlesProducer::Produce(event_type const& event, product_type& product, KappaSettings const& settings) const
{
	for (std::vector<KGenParticle*>::iterator genParticle = (product.*m_genParticlesMember).begin();
	     genParticle != (product.*m_genParticlesMember).end();)
	{
		bool validLepton = (std::abs((*genParticle)->pdgId) == m_absPdgId);
		
		// kinematic cuts
		validLepton = validLepton && this->PassKinematicCuts(*genParticle, event, product);
		
		// check possible analysis-specific criteria
		validLepton = validLepton && AdditionalCriteria(*genParticle, event, product, settings);
		
		if (validLepton)
		{
			(product.*m_validLeptonsMember).push_back(*genParticle);
			++genParticle;
		}
		else
		{
			genParticle = (product.*m_genParticlesMember).erase(genParticle);
		}
	}
	
	// preserve Pt sorting of valid gen. leptons
	std::sort(
			(product.*m_validLeptonsMember).begin(),
			(product.*m_validLeptonsMember).end(),
			[](KGenParticle const* genParticle1, KGenParticle const* genParticle2) -> bool
			{
				return genParticle1->p4.Pt() > genParticle2->p4.Pt();
			}
	);
	std::sort(
			(product.*m_genParticlesMember).begin(),
			(product.*m_genParticlesMember).end(),
			[](KGenParticle const* genParticle1, KGenParticle const* genParticle2) -> bool
			{
				return genParticle1->p4.Pt() > genParticle2->p4.Pt();
			}
	);
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

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genTau1DecayMode", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_validGenTaus.size() > 0 ? SafeMap::Get(product.m_validGenTausMap, product.m_validGenTaus.at(0))->genDecayMode() : DefaultValues::UndefinedInt);
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genTau2DecayMode", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_validGenTaus.size() > 1 ? SafeMap::Get(product.m_validGenTausMap, product.m_validGenTaus.at(1))->genDecayMode() : DefaultValues::UndefinedInt);
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genTau1NProngs", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_validGenTaus.size() > 0 ? SafeMap::Get(product.m_validGenTausMap, product.m_validGenTaus.at(0))->nProngs : DefaultValues::UndefinedInt);
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genTau2ProngSize", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_validGenTaus.size() > 1 ? SafeMap::Get(product.m_validGenTausMap, product.m_validGenTaus.at(1))->nPi0s : DefaultValues::UndefinedInt);
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genTau1NPi0s", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_validGenTaus.size() > 0 ? SafeMap::Get(product.m_validGenTausMap, product.m_validGenTaus.at(0))->nPi0s : DefaultValues::UndefinedInt);
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genTau2Pi0s", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_validGenTaus.size() > 1 ? SafeMap::Get(product.m_validGenTausMap, product.m_validGenTaus.at(1))->nProngs : DefaultValues::UndefinedInt);
	});
}

void ValidGenTausProducer::Produce(event_type const& event, product_type& product, KappaSettings const& settings) const
{
	ValidGenParticlesProducer::Produce(event, product, settings);
	
	// matching of KGenParticle to KGenTau
	for (std::vector<KGenParticle*>::iterator genParticle = (product.*m_validLeptonsMember).begin();
	     genParticle != (product.*m_validLeptonsMember).end(); ++genParticle)
	{
		float minAbsDeltaR = std::numeric_limits<float>::max();
		float minAbsDeltaPt = std::numeric_limits<float>::max();
		KGenTau* bestMatchingGenTau = nullptr;
		
		for (std::vector<KGenTau>::iterator genTau = event.m_genTaus->begin();
		     genTau != event.m_genTaus->end(); ++genTau)
		{
			float absDeltaR = ROOT::Math::VectorUtil::DeltaR((*genParticle)->p4, genTau->p4);
			float absDeltaPt = std::abs((*genParticle)->p4.Pt() - genTau->p4.Pt());
			if ((absDeltaR < minAbsDeltaR) || (absDeltaPt < minAbsDeltaPt))
			{
				minAbsDeltaR = absDeltaR;
				minAbsDeltaPt = absDeltaPt;
				bestMatchingGenTau = &(*genTau);
			}
		}
		product.m_validGenTausMap[*genParticle] = bestMatchingGenTau;
	}
}

