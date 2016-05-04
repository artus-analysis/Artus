
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"

#include "Artus/KappaAnalysis/interface/Producers/GenDiLeptonDecayModeProducer.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"
#include "Artus/Utility/interface/Utility.h"


std::string GenDiLeptonDecayModeProducer::GetProducerId() const {
	return "GenDiLeptonDecayModeProducer";
}

void GenDiLeptonDecayModeProducer::Init(KappaSettings const& settings)
{
	ProducerBase<KappaTypes>::Init(settings);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genDiLeptonBosonMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genDiLeptonBoson.mass();
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZEE", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EE);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZMM", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MM);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZLL", [](KappaEvent const& event, KappaProduct const& product)
	{
		return ((product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MM) || (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EE));
	});
}

void GenDiLeptonDecayModeProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                           KappaSettings const& settings) const
{
	assert(event.m_genParticles);

	for (KGenParticles::const_iterator genParticle = event.m_genParticles->begin();
		 genParticle != event.m_genParticles->end(); ++genParticle)
	{
		if (Utility::Contains(settings.GetBosonPdgIds(), std::abs(genParticle->pdgId)) && Utility::Contains(settings.GetBosonStatuses(), genParticle->status()))
		{
			std::map<int, int> nDecayProductsPerType;
			product.m_genDiLeptonBoson = (*genParticle).p4;

			for (std::vector<unsigned int>::const_iterator decayParticleIndex = genParticle->daughterIndices.begin();
			     decayParticleIndex != genParticle->daughterIndices.end(); ++decayParticleIndex)
			{
				int pdgId = std::abs(event.m_genParticles->at(*decayParticleIndex).pdgId);
				nDecayProductsPerType[pdgId] = SafeMap::GetWithDefault(nDecayProductsPerType, pdgId, 0) + 1;
			}

			if (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdElectron, 0) >= 2)
			{
				product.m_genDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::EE;
			}
			else if (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdMuon, 0) >= 2)
			{
				product.m_genDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::MM;
			}
			else if (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdTau, 0) >= 2)
			{
				product.m_genDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::TT;
			}
			else
			{
				product.m_genDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::NONE;
			}

			// exit loop after one boson is found
			if (product.m_genDiLeptonDecayMode != KappaEnumTypes::DiLeptonDecayMode::NONE)
			{
				break;
			}
		}
	}
	
	// If no boson has been found in the event, try to reconstruct it from the first two decay
	// products available in the list of gen. particles
	if (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::NONE)
	{
		size_t iDaughter = 0;
		RMFLV genDiLeptonBoson;
		
		for (KGenParticles::const_iterator genParticle = event.m_genParticles->begin();
		 genParticle != event.m_genParticles->end() && (iDaughter < 2); ++genParticle)
		{
			if (std::abs(genParticle->pdgId) == DefaultValues::pdgIdElectron ||
			    std::abs(genParticle->pdgId) == DefaultValues::pdgIdMuon ||
			    std::abs(genParticle->pdgId) == DefaultValues::pdgIdTau)
// 			if (genParticle->isPrompt() && genParticle->isPromptDecayed())
			{
				genDiLeptonBoson += (*genParticle).p4;
				iDaughter++;
			}
		}
		
		product.m_genDiLeptonBoson = genDiLeptonBoson;
	}
}
