
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
                                                     std::vector<std::string>& (setting_type::*GetUpperAbsEtaCuts)(void) const,
                                                     std::string name) :
	ProducerBase<KappaTypes>(),
	ValidPhysicsObjectTools<KappaTypes, KGenParticle>(GetLowerPtCuts, GetUpperAbsEtaCuts, validLeptons),
	m_validLeptonsMember(validLeptons),
	m_name(name),
	m_genParticlesMember(genParticles),
	m_absPdgId(absPdgId)
{
}

void ValidGenParticlesProducer::Init(KappaSettings const& settings)
{
	ProducerBase<KappaTypes>::Init(settings);

	// add possible quantities for the lambda ntuples consumers
	for (size_t leptonIndex = 0; leptonIndex < 2; ++leptonIndex)
	{
		std::string quantityNameBase = "gen" + m_name + std::to_string(leptonIndex+1);
		
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(quantityNameBase+"Pt", [this, leptonIndex](event_type const& event, product_type const& product)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? (product.*m_validLeptonsMember)[leptonIndex]->p4.Pt() : DefaultValues::UndefinedFloat);
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(quantityNameBase+"Eta", [this, leptonIndex](event_type const& event, product_type const& product)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? (product.*m_validLeptonsMember)[leptonIndex]->p4.Eta() : DefaultValues::UndefinedFloat);
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(quantityNameBase+"Phi", [this, leptonIndex](event_type const& event, product_type const& product)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? (product.*m_validLeptonsMember)[leptonIndex]->p4.Phi() : DefaultValues::UndefinedFloat);
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(quantityNameBase+"Mass", [this, leptonIndex](event_type const& event, product_type const& product)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? (product.*m_validLeptonsMember)[leptonIndex]->p4.mass() : DefaultValues::UndefinedFloat);
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(quantityNameBase+"Charge", [this, leptonIndex](event_type const& event, product_type const& product)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? (product.*m_validLeptonsMember)[leptonIndex]->charge() : DefaultValues::UndefinedFloat);
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(quantityNameBase+"LV", [this, leptonIndex](event_type const& event, product_type const& product)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? (product.*m_validLeptonsMember)[leptonIndex]->p4 : DefaultValues::UndefinedRMFLV);
		});
	}
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
			++genParticle; // genParticle = (product.*m_genParticlesMember).erase(genParticle);
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
	                          &setting_type::GetGenElectronUpperAbsEtaCuts,
	                          "Electron")
{
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
	                          &setting_type::GetGenMuonUpperAbsEtaCuts,
	                          "Muon")
{
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
	                          &setting_type::GetGenTauUpperAbsEtaCuts,
	                          "Tau")
{
}

void ValidGenTausProducer::Init(KappaSettings const& settings)
{
	ValidGenParticlesProducer::Init(settings);

	// add possible quantities for the lambda ntuples consumers
	for (size_t leptonIndex = 0; leptonIndex < 2; ++leptonIndex)
	{
		std::string quantityNameBase = "gen" + m_name + std::to_string(leptonIndex+1);
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(quantityNameBase+"VisibleLV", [this, leptonIndex](event_type const& event, product_type const& product)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? static_cast<KGenTau*>((product.*m_validLeptonsMember)[leptonIndex])->visible.p4 : DefaultValues::UndefinedRMFLV);
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(quantityNameBase+"DecayMode", [this, leptonIndex](event_type const& event, product_type const& product)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? static_cast<KGenTau*>((product.*m_validLeptonsMember)[leptonIndex])->genDecayMode() : DefaultValues::UndefinedInt);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(quantityNameBase+"NProngs", [this, leptonIndex](event_type const& event, product_type const& product)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? static_cast<KGenTau*>((product.*m_validLeptonsMember)[leptonIndex])->nProngs : DefaultValues::UndefinedInt);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(quantityNameBase+"NPi0s", [this, leptonIndex](event_type const& event, product_type const& product)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? static_cast<KGenTau*>((product.*m_validLeptonsMember)[leptonIndex])->nPi0s : DefaultValues::UndefinedInt);
		});
	}
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

