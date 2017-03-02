
#include "Artus/KappaAnalysis/interface/Producers/GenBosonProducers.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/Utility/interface/Utility.h"
#include "Artus/Utility/interface/DefaultValues.h"


std::string GenBosonFromGenParticlesProducer::GetProducerId() const {
	return "GenBosonFromGenParticlesProducer";
}

void GenBosonFromGenParticlesProducer::Init(KappaSettings const& settings)
{
	ProducerBase<KappaTypes>::Init(settings);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("genBosonParticleFound", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBosonParticle != nullptr);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("genBosonLV", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV;
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonPt", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV.Pt();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonEta", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV.Eta();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonPhi", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV.Phi();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV.mass();
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("genBosonLVFound", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLVFound;
	});
}

void GenBosonFromGenParticlesProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                               KappaSettings const& settings) const
{
	assert(event.m_genParticles);
	product.m_genBosonLVFound = false;
	
	for (KGenParticles::const_iterator genParticle = event.m_genParticles->begin();
		 genParticle != event.m_genParticles->end(); ++genParticle)
	{
		if (Utility::Contains(settings.GetBosonPdgIds(), std::abs(genParticle->pdgId)) && Utility::Contains(settings.GetBosonStatuses(), genParticle->status()))
		{
			product.m_genBosonParticle = const_cast<KGenParticle*>(&(*genParticle));
			product.m_genBosonLV = (*genParticle).p4;
			product.m_genBosonLVFound = true;
			break;
		}
	}
}


std::string GenBosonProductionProducer::GetProducerId() const {
	return "GenBosonProductionProducer";
}

void GenBosonProductionProducer::Init(KappaSettings const& settings)
{
	GenBosonFromGenParticlesProducer::Init(settings);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("genBosonParticleFound", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBosonParticle != nullptr);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity("genBosonLV", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV;
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonPt", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV.Pt();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonEta", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV.Eta();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonPhi", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV.Phi();
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genBosonMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLV.mass();
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("genBosonLVFound", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBosonLVFound;
	});
}

void GenBosonProductionProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                         KappaSettings const& settings) const
{
	GenBosonFromGenParticlesProducer::Produce(event, product, settings);
	assert(product.m_genBosonParticle != nullptr);
	
	// search for boson index
	unsigned int bosonIndex = 0;
	for (KGenParticles::const_iterator genParticle = event.m_genParticles->begin();
		 genParticle != event.m_genParticles->end(); ++genParticle)
	{
		if (product.m_genBosonParticle == &(*genParticle))
		{
			break;
		}
		++bosonIndex;
	}
	
	product.m_genParticlesProducingBoson = FindMothersWithDifferentPdgId(event.m_genParticles, bosonIndex, product.m_genBosonParticle->pdgId);
	for (size_t i = 0; i < product.m_genParticlesProducingBoson.size(); ++i)
	{
		LOG(WARNING) << product.m_genParticlesProducingBoson[i]->p4 << ", " << product.m_genParticlesProducingBoson[i]->pdgId;
	}
}

std::vector<KGenParticle*> GenBosonProductionProducer::FindMothersWithDifferentPdgId(
		KGenParticles* genParticles,
		unsigned int currentIndex,
		int currentPdgId) const
{
	std::vector<KGenParticle*> mothers;
	
	unsigned int index = 0;
	for (KGenParticles::iterator genParticle = genParticles->begin();
		 genParticle != genParticles->end(); ++genParticle)
	{
		if (Utility::Contains(genParticle->daughterIndices, currentIndex))
		{
			if (genParticle->pdgId == currentPdgId)
			{
				std::vector<KGenParticle*> tmpMothers = FindMothersWithDifferentPdgId(genParticles, index, currentPdgId);
				mothers.insert(mothers.end(), tmpMothers.begin(), tmpMothers.end());
			}
			else
			{
				mothers.push_back(&(*genParticle));
			}
		}
		++index;
	}
	
	return mothers;
}


std::string GenBosonDiLeptonDecayModeProducer::GetProducerId() const {
	return "GenBosonDiLeptonDecayModeProducer";
}

void GenBosonDiLeptonDecayModeProducer::Init(KappaSettings const& settings)
{
	GenBosonFromGenParticlesProducer::Init(settings);

	// add possible quantities for the lambda ntuples consumers
	for (size_t leptonIndex = 0; leptonIndex < 2; ++leptonIndex)
	{
		std::string lepQuantityNameBase = "genBosonLep" + std::to_string(leptonIndex+1);
		std::string tauQuantityNameBase = "genBosonTau" + std::to_string(leptonIndex+1);
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(lepQuantityNameBase+"LV", [leptonIndex](event_type const& event, product_type const& product)
		{
			return product.m_genLeptonsFromBosonDecay.at(leptonIndex)->p4;
		});
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(tauQuantityNameBase+"LV", [leptonIndex](event_type const& event, product_type const& product)
		{
			return ((std::abs(product.m_genLeptonsFromBosonDecay.at(leptonIndex)->pdgId) == DefaultValues::pdgIdTau) ? product.m_genLeptonsFromBosonDecay.at(leptonIndex)->p4 : DefaultValues::UndefinedRMFLV);
		});
		
		LambdaNtupleConsumer<KappaTypes>::AddRMFLVQuantity(tauQuantityNameBase+"VisibleLV", [leptonIndex](event_type const& event, product_type const& product)
		{
			if (std::abs(product.m_genLeptonsFromBosonDecay.at(leptonIndex)->pdgId) == DefaultValues::pdgIdTau)
			{
				KGenTau* genTau = SafeMap::GetWithDefault(product.m_validGenTausMap, product.m_genLeptonsFromBosonDecay.at(leptonIndex), static_cast<KGenTau*>(nullptr));
				return (genTau ? genTau->visible.p4 : DefaultValues::UndefinedRMFLV);
			}
			else
			{
				return DefaultValues::UndefinedRMFLV;
			}
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(tauQuantityNameBase+"DecayMode", [leptonIndex](event_type const& event, product_type const& product)
		{
			if (std::abs(product.m_genLeptonsFromBosonDecay.at(leptonIndex)->pdgId) == DefaultValues::pdgIdTau)
			{
				KGenTau* genTau = SafeMap::GetWithDefault(product.m_validGenTausMap, product.m_genLeptonsFromBosonDecay.at(leptonIndex), static_cast<KGenTau*>(nullptr));
				return (genTau ? genTau->genDecayMode() : DefaultValues::UndefinedInt);
			}
			else
			{
				return DefaultValues::UndefinedInt;
			}
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(tauQuantityNameBase+"NProngs", [leptonIndex](event_type const& event, product_type const& product)
		{
			if (std::abs(product.m_genLeptonsFromBosonDecay.at(leptonIndex)->pdgId) == DefaultValues::pdgIdTau)
			{
				KGenTau* genTau = SafeMap::GetWithDefault(product.m_validGenTausMap, product.m_genLeptonsFromBosonDecay.at(leptonIndex), static_cast<KGenTau*>(nullptr));
				return (genTau ? genTau->nProngs : DefaultValues::UndefinedInt);
			}
			else
			{
				return DefaultValues::UndefinedInt;
			}
		});
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(tauQuantityNameBase+"NPi0s", [leptonIndex](event_type const& event, product_type const& product)
		{
			if (std::abs(product.m_genLeptonsFromBosonDecay.at(leptonIndex)->pdgId) == DefaultValues::pdgIdTau)
			{
				KGenTau* genTau = SafeMap::GetWithDefault(product.m_validGenTausMap, product.m_genLeptonsFromBosonDecay.at(leptonIndex), static_cast<KGenTau*>(nullptr));
				return (genTau ? genTau->nPi0s : DefaultValues::UndefinedInt);
			}
			else
			{
				return DefaultValues::UndefinedInt;
			}
		});
	}
}

void GenBosonDiLeptonDecayModeProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                                KappaSettings const& settings) const
{
	GenBosonFromGenParticlesProducer::Produce(event, product, settings);
	
	// If no boson has been found in the event, try to reconstruct it from the first two decay
	// products available in the list of gen. particles
	// https://hypernews.cern.ch/HyperNews/CMS/get/generators/2802/1.html
	if (product.m_genBosonParticle == nullptr)
	{
		size_t iDaughter = 0;
		RMFLV genBosonLV;
		
		for (KGenParticles::iterator genParticle = event.m_genParticles->begin();
		     genParticle != event.m_genParticles->end() && (iDaughter < 2); ++genParticle)
		{
			if ((std::abs(genParticle->pdgId) == DefaultValues::pdgIdElectron) ||
			    (std::abs(genParticle->pdgId) == DefaultValues::pdgIdMuon) ||
			    (std::abs(genParticle->pdgId) == DefaultValues::pdgIdTau))
			// if (genParticle->isPrompt() && genParticle->isPromptDecayed())
			{
				genBosonLV += (*genParticle).p4;
				product.m_genLeptonsFromBosonDecay.push_back(&(*genParticle));
				++iDaughter;
			}
		}
		
		product.m_genBosonLV = genBosonLV;
		product.m_genBosonLVFound = (iDaughter == 2);
	}
	else
	{
		for (std::vector<unsigned int>::const_iterator decayParticleIndex = product.m_genBosonParticle->daughterIndices.begin();
		     decayParticleIndex != product.m_genBosonParticle->daughterIndices.end(); ++decayParticleIndex)
		{
			int pdgId = std::abs(event.m_genParticles->at(*decayParticleIndex).pdgId);
			if ((pdgId == DefaultValues::pdgIdElectron) ||
			    (pdgId == DefaultValues::pdgIdMuon) ||
			    (pdgId == DefaultValues::pdgIdTau))
			{
				product.m_genLeptonsFromBosonDecay.push_back(&(event.m_genParticles->at(*decayParticleIndex)));
			}
		}
	}
}
