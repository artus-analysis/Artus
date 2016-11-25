
#include "Artus/KappaAnalysis/interface/Producers/GenBosonProducers.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/Utility/interface/Utility.h"
#include "Artus/Utility/interface/DefaultValues.h"


void GenBosonFromGenParticlesProducer::Init(KappaSettings const& settings)
{
	ProducerBase<KappaTypes>::Init(settings);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("genBosonParticleFound", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBosonParticle != nullptr);
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


void GenBosonDiLeptonDecayModeProducer::Init(KappaSettings const& settings)
{
	GenBosonFromGenParticlesProducer::Init(settings);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genDiLeptonBosonMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBosonLVFound ? product.m_genBosonLV.mass() : DefaultValues::UndefinedFloat);
	});
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
