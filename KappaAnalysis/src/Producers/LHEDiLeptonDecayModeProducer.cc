
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"

#include "Artus/KappaAnalysis/interface/Producers/LHEDiLeptonDecayModeProducer.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"
#include "Artus/Utility/interface/Utility.h"


std::string LHEDiLeptonDecayModeProducer::GetProducerId() const {
	return "LHEDiLeptonDecayModeProducer";
}

void LHEDiLeptonDecayModeProducer::Init(KappaSettings const& settings)
{
	ProducerBase<KappaTypes>::Init(settings);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("lheZtoEM", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EM);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("lheZtoET", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::ET);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("lheZtoMT", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MT);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("lheZtoEE", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EE);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("lheZtoMM", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MM);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("lheZtoTT", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::TT);
	});

	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("lheZtoLL", [](KappaEvent const& event, KappaProduct const& product)
	{
		return ((product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MM) || (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EE) || (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EM));
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("lheDiLeptonDecayMode", [](KappaEvent const& event, KappaProduct const& product)
	{
		return Utility::ToUnderlyingValue(product.m_lheDiLeptonDecayMode);
	});
}

void LHEDiLeptonDecayModeProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                           KappaSettings const& settings) const
{
	product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::NONE;

	assert(event.m_lheParticles);

	//std::cout<< event.m_lheParticles->particles.size()<<std::endl;
	//std::cout<< event.m_lheParticles->particles[3].pdgId<<std::endl;
	//std::cout<< event.m_lheParticles->particles[4].pdgId<<std::endl;
	if ((event.m_lheParticles->particles[3].status != 1) || (event.m_lheParticles->particles[4].status != 1))
	{
		std::cout<< event.m_lheParticles->particles[3].status<<std::endl;
		std::cout<< event.m_lheParticles->particles[4].status<<std::endl;
	}

	if (event.m_lheParticles->particles[3].pdgId == -(event.m_lheParticles->particles[4].pdgId))
	{
		std::cout<< "no LFV" <<std::endl;
		std::cout<< event.m_lheParticles->particles[3].pdgId<<std::endl;
		std::cout<< event.m_lheParticles->particles[4].pdgId<<std::endl;
	}
	if ((event.m_lheParticles->particles[3].pdgId == 11) || (event.m_lheParticles->particles[3].pdgId == -11))
	{
		if ((event.m_lheParticles->particles[4].pdgId == 11) || (event.m_lheParticles->particles[4].pdgId == -11))
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::EE;
		}

		if ((event.m_lheParticles->particles[4].pdgId == 13) || (event.m_lheParticles->particles[4].pdgId == -13))
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::EM;
		}

		if ((event.m_lheParticles->particles[4].pdgId == 15) || (event.m_lheParticles->particles[4].pdgId == -15))
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::ET;
		}
	}

	if ((event.m_lheParticles->particles[3].pdgId == 13) || (event.m_lheParticles->particles[3].pdgId == -13))
	{
		if ((event.m_lheParticles->particles[4].pdgId == 11) || (event.m_lheParticles->particles[4].pdgId == -11))
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::EM;
		}

		if ((event.m_lheParticles->particles[4].pdgId == 13) || (event.m_lheParticles->particles[4].pdgId == -13))
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::MM;
		}

		if ((event.m_lheParticles->particles[4].pdgId == 15) || (event.m_lheParticles->particles[4].pdgId == -15))
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::MT;
		}
	}

	if ((event.m_lheParticles->particles[3].pdgId == 15) || (event.m_lheParticles->particles[3].pdgId == -15))
	{
		if ((event.m_lheParticles->particles[4].pdgId == 11) || (event.m_lheParticles->particles[4].pdgId == -11))
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::ET;
		}

		if ((event.m_lheParticles->particles[4].pdgId == 13) || (event.m_lheParticles->particles[4].pdgId == -13))
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::MT;
		}

		if ((event.m_lheParticles->particles[4].pdgId == 15) || (event.m_lheParticles->particles[4].pdgId == -15))
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::TT;
		}
	}
	/*std::map<int, int> nDecayProductsPerType;
	size_t iDaughter = 0;
	for (std::vector<KLHEParticle*>::iterator lheParticle = product.m_lheLeptonsFromBosonDecay.begin();
	     lheParticle != product.m_lheLeptonsFromBosonDecay.end() && (iDaughter < 2); ++lheParticle)
	{
		int pdgId = std::abs((*lheParticle)->pdgId);
		nDecayProductsPerType[pdgId] = SafeMap::GetWithDefault(nDecayProductsPerType, pdgId, 0) + 1;
		++iDaughter;
	}
	
	if ((SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdElectron, 0) == 1) && (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdMuon, 0) == 1))
	{
		product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::EM;
	}
	else if ((SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdMuon, 0) == 1) && (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdTau, 0) == 1))
	{
		product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::MT;
	}
	else if ((SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdElectron, 0) == 1) && (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdTau, 0) == 1))
	{
		product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::ET;
	}
	*/
}
