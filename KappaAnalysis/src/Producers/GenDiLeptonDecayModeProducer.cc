
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"

#include "Artus/KappaAnalysis/interface/Producers/GenDiLeptonDecayModeProducer.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"


std::string GenDiLeptonDecayModeProducer::GetProducerId() const {
	return "GenDiLeptonDecayModeProducer";
}

void GenDiLeptonDecayModeProducer::Init(KappaSettings const& settings)
{
	ProducerBase<KappaTypes>::Init(settings);
}

void GenDiLeptonDecayModeProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                           KappaSettings const& settings) const
{
	assert(event.m_genParticles);

	for (KGenParticles::const_iterator genParticle = event.m_genParticles->begin();
		 genParticle != event.m_genParticles->end(); ++genParticle)
	{
		if ((std::abs(genParticle->pdgId()) == settings.GetBosonPdgId()) && (genParticle->status() == settings.GetBosonStatus()))
		{
			std::map<int, int> nDecayProductsPerType;

			for (std::vector<unsigned int>::const_iterator decayParticleIndex = genParticle->daughterIndices.begin();
			     decayParticleIndex != genParticle->daughterIndices.end(); ++decayParticleIndex)
			{
				int pdgId = std::abs(event.m_genParticles->at(*decayParticleIndex).pdgId());
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
}

