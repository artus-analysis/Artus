
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"

#include "Artus/KappaAnalysis/interface/Producers/LHEDiLeptonDecayModeProducer.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"
#include "Artus/Utility/interface/Utility.h"


std::string LHEDiLeptonDecayModeProducer::GetProducerId() const {
	return "LHEDiLeptonDecayModeProducer";
}

void LHEDiLeptonDecayModeProducer::Init(setting_type const& settings, metadata_type& metadata)
{
	ProducerBase<KappaTypes>::Init(settings, metadata);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZtoEM", [](event_type const& event, product_type const& product)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EM);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZtoET", [](event_type const& event, product_type const& product)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::ET);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZtoMT", [](event_type const& event, product_type const& product)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MT);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZtoEE", [](event_type const& event, product_type const& product)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EE);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZtoMM", [](event_type const& event, product_type const& product)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MM);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZtoTT", [](event_type const& event, product_type const& product)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::TT);
	});

	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZtoLL", [](event_type const& event, product_type const& product)
	{
		return ((product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MM) || (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EE) || (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EM));
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "lheDiLeptonDecayMode", [](event_type const& event, product_type const& product)
	{
		return Utility::ToUnderlyingValue(product.m_lheDiLeptonDecayMode);
	});
}

void LHEDiLeptonDecayModeProducer::Produce(event_type const& event, product_type& product,
                                           setting_type const& settings, metadata_type const& metadata) const
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
