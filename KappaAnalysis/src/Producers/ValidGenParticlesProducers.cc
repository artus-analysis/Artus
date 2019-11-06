
#include "Artus/KappaAnalysis/interface/Producers/ValidGenParticlesProducers.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"

#include <limits>

#include <Math/VectorUtil.h>


ValidGenParticlesProducer::ValidGenParticlesProducer(std::vector<KGenParticle*> KappaTypes::product_type::*genParticles,
                                                     int absPdgId,
                                                     std::vector<KGenParticle*> KappaTypes::product_type::*validLeptons,
                                                     std::vector<std::string>& (KappaTypes::setting_type::*GetLowerPtCuts)(void) const,
                                                     std::vector<std::string>& (KappaTypes::setting_type::*GetUpperAbsEtaCuts)(void) const,
                                                     std::string name) :
	ProducerBase<KappaTypes>(),
	ValidPhysicsObjectTools<KappaTypes, KGenParticle>(GetLowerPtCuts, GetUpperAbsEtaCuts, validLeptons),
	m_validLeptonsMember(validLeptons),
	m_name(name),
	m_genParticlesMember(genParticles),
	m_absPdgId(absPdgId)
{
}

void ValidGenParticlesProducer::Init(KappaTypes::KappaTypes::setting_type const& settings, KappaTypes::KappaTypes::metadata_type& metadata)
{
	ProducerBase<KappaTypes>::Init(settings, metadata);

	// add possible quantities for the lambda ntuples consumers
	for (size_t leptonIndex = 0; leptonIndex < 2; ++leptonIndex)
	{
		std::string quantityNameBase = "gen" + m_name + std::to_string(leptonIndex+1);

		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, quantityNameBase+"Pt", [this, leptonIndex](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? (product.*m_validLeptonsMember)[leptonIndex]->p4.Pt() : DefaultValues::UndefinedFloat);
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, quantityNameBase+"Eta", [this, leptonIndex](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? (product.*m_validLeptonsMember)[leptonIndex]->p4.Eta() : DefaultValues::UndefinedFloat);
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, quantityNameBase+"Phi", [this, leptonIndex](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? (product.*m_validLeptonsMember)[leptonIndex]->p4.Phi() : DefaultValues::UndefinedFloat);
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, quantityNameBase+"Mass", [this, leptonIndex](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? (product.*m_validLeptonsMember)[leptonIndex]->p4.mass() : DefaultValues::UndefinedFloat);
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, quantityNameBase+"Charge", [this, leptonIndex](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? (product.*m_validLeptonsMember)[leptonIndex]->charge() : DefaultValues::UndefinedFloat);
		});

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(metadata, quantityNameBase+"LV", [this, leptonIndex](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? (product.*m_validLeptonsMember)[leptonIndex]->p4 : DefaultValues::UndefinedRMFLV);
		});
	}
}

void ValidGenParticlesProducer::Produce(KappaTypes::KappaTypes::event_type const& event, KappaTypes::KappaTypes::product_type& product,
                                        KappaTypes::KappaTypes::setting_type const& settings, KappaTypes::KappaTypes::metadata_type const& metadata) const
{
	for (std::vector<KGenParticle*>::iterator genParticle = (product.*m_genParticlesMember).begin();
	     genParticle != (product.*m_genParticlesMember).end();)
	{
		bool validLepton = (std::abs((*genParticle)->pdgId) == m_absPdgId);

		// kinematic cuts
		validLepton = validLepton && this->PassKinematicCuts(*genParticle, event, product);

		// check possible analysis-specific criteria
		validLepton = validLepton && AdditionalCriteria(*genParticle, event, product, settings, metadata);

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

bool ValidGenParticlesProducer::AdditionalCriteria(KGenParticle* genParticle, KappaTypes::KappaTypes::event_type const& event, KappaTypes::KappaTypes::product_type& product,
                                                   KappaTypes::KappaTypes::setting_type const& settings, KappaTypes::KappaTypes::metadata_type const& metadata) const
{
	bool validLepton = true;
	return validLepton;
}


ValidGenElectronsProducer::ValidGenElectronsProducer() :
	ValidGenParticlesProducer(&KappaTypes::product_type::m_genLeptonsFromBosonDecay,
	                          DefaultValues::pdgIdElectron,
	                          &KappaTypes::product_type::m_validGenElectrons,
	                          &KappaTypes::setting_type::GetGenElectronLowerPtCuts,
	                          &KappaTypes::setting_type::GetGenElectronUpperAbsEtaCuts,
	                          "Electron")
{
}

std::string ValidGenElectronsProducer::GetProducerId() const {
	return "ValidGenElectronsProducer";
}

void ValidGenElectronsProducer::Init(KappaTypes::KappaTypes::setting_type const& settings, KappaTypes::KappaTypes::metadata_type& metadata)
{
	ValidGenParticlesProducer::Init(settings, metadata);
}


ValidGenMuonsProducer::ValidGenMuonsProducer() :
	ValidGenParticlesProducer(&KappaTypes::product_type::m_genLeptonsFromBosonDecay,
	                          DefaultValues::pdgIdMuon,
	                          &KappaTypes::product_type::m_validGenMuons,
	                          &KappaTypes::setting_type::GetGenMuonLowerPtCuts,
	                          &KappaTypes::setting_type::GetGenMuonUpperAbsEtaCuts,
	                          "Muon")
{
}

std::string ValidGenMuonsProducer::GetProducerId() const {
	return "ValidGenMuonsProducer";
}

void ValidGenMuonsProducer::Init(KappaTypes::KappaTypes::setting_type const& settings, KappaTypes::KappaTypes::metadata_type& metadata)
{
	ValidGenParticlesProducer::Init(settings, metadata);
}


ValidGenTausProducer::ValidGenTausProducer() :
	ValidGenParticlesProducer(&KappaTypes::product_type::m_genLeptonsFromBosonDecay,
	                          DefaultValues::pdgIdTau,
	                          &KappaTypes::product_type::m_validGenTaus,
	                          &KappaTypes::setting_type::GetGenTauLowerPtCuts,
	                          &KappaTypes::setting_type::GetGenTauUpperAbsEtaCuts,
	                          "Tau")
{
}

std::string ValidGenTausProducer::GetProducerId() const {
	return "ValidGenTausProducer";
}

void ValidGenTausProducer::Init(KappaTypes::KappaTypes::setting_type const& settings, KappaTypes::KappaTypes::metadata_type& metadata)
{
	ValidGenParticlesProducer::Init(settings, metadata);

	// add possible quantities for the lambda ntuples consumers
	for (size_t leptonIndex = 0; leptonIndex < 2; ++leptonIndex)
	{
		std::string quantityNameBase = "gen" + m_name + std::to_string(leptonIndex+1);

		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(metadata, quantityNameBase+"VisibleLV", [this, leptonIndex](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? static_cast<KGenTau*>((product.*m_validLeptonsMember)[leptonIndex])->visible.p4 : DefaultValues::UndefinedRMFLV);
		});

		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, quantityNameBase+"DecayMode", [this, leptonIndex](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? static_cast<KGenTau*>((product.*m_validLeptonsMember)[leptonIndex])->genDecayMode() : DefaultValues::UndefinedInt);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, quantityNameBase+"NProngs", [this, leptonIndex](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? static_cast<KGenTau*>((product.*m_validLeptonsMember)[leptonIndex])->nProngs : DefaultValues::UndefinedInt);
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, quantityNameBase+"NPi0s", [this, leptonIndex](KappaTypes::event_type const& event, KappaTypes::product_type const& product, KappaTypes::setting_type const& settings, KappaTypes::metadata_type const& metadata)
		{
			return (((product.*m_validLeptonsMember).size() > leptonIndex) ? static_cast<KGenTau*>((product.*m_validLeptonsMember)[leptonIndex])->nPi0s : DefaultValues::UndefinedInt);
		});
	}
}

void ValidGenTausProducer::Produce(KappaTypes::KappaTypes::event_type const& event, KappaTypes::KappaTypes::product_type& product,
                                   KappaTypes::KappaTypes::setting_type const& settings, KappaTypes::KappaTypes::metadata_type const& metadata) const
{
	ValidGenParticlesProducer::Produce(event, product, settings, metadata);

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
		product.m_validGenParticlesMap[bestMatchingGenTau] = *genParticle;

		std::pair<std::vector<KGenParticle*>, std::vector<KGenParticle*> > chargedNeutralHadrons = GetChargedNeutralHadrons(event.m_genParticles, *genParticle);
		product.m_validGenTausChargedHadronsMap[*genParticle] = chargedNeutralHadrons.first;
		product.m_validGenTausNeutralHadronsMap[*genParticle] = chargedNeutralHadrons.second;
	}
}

std::pair<std::vector<KGenParticle*>, std::vector<KGenParticle*> > ValidGenTausProducer::GetChargedNeutralHadrons(KGenParticles* genParticles, KGenParticle* genTau) const
{
	std::pair<std::vector<KGenParticle*>, std::vector<KGenParticle*> > chargedNeutralHadrons = std::pair<std::vector<KGenParticle*>, std::vector<KGenParticle*> >(
			std::vector<KGenParticle*>(), std::vector<KGenParticle*>()
	);

	for (std::vector<unsigned int>::iterator daughterIndex = genTau->daughterIndices.begin(); daughterIndex != genTau->daughterIndices.end(); ++daughterIndex)
	{
		KGenParticle* daughterGenParticle = &(genParticles->at(*daughterIndex));
		int absPdgId = std::abs(daughterGenParticle->pdgId);
		if (absPdgId == DefaultValues::pdgIdPiPlus) // maybe add charged leptons here as well
		{
			chargedNeutralHadrons.first.push_back(daughterGenParticle);
		}
		else if (absPdgId == DefaultValues::pdgIdPiZero)
		{
			chargedNeutralHadrons.second.push_back(daughterGenParticle);
		}
		else if (absPdgId != DefaultValues::pdgIdNuTau)
		{
			std::pair<std::vector<KGenParticle*>, std::vector<KGenParticle*> > tmpChargedNeutralHadrons = GetChargedNeutralHadrons(genParticles, daughterGenParticle);
			chargedNeutralHadrons.first.insert(chargedNeutralHadrons.first.end(),
			                                   tmpChargedNeutralHadrons.first.begin(), tmpChargedNeutralHadrons.first.end());
			chargedNeutralHadrons.second.insert(chargedNeutralHadrons.second.end(),
			                                    tmpChargedNeutralHadrons.second.begin(), tmpChargedNeutralHadrons.second.end());
		}
	}

	return chargedNeutralHadrons;
}
