
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
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZEE", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EE);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZMM", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MM);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZTT", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::TT);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZLL", [](KappaEvent const& event, KappaProduct const& product)
	{
		return ((product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MM) || (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EE));
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genDiLeptonDecayMode", [](KappaEvent const& event, KappaProduct const& product)
	{
		return Utility::ToUnderlyingValue(product.m_genDiLeptonDecayMode);
	});
}

void GenDiLeptonDecayModeProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                           KappaSettings const& settings) const
{
	std::map<int, int> nDecayProductsPerType;
	size_t iDaughter = 0;
	for (std::vector<KGenParticle*>::iterator genParticle = product.m_genLeptonsFromBosonDecay.begin();
	     genParticle != product.m_genLeptonsFromBosonDecay.end() && (iDaughter < 2); ++genParticle)
	{
		int pdgId = std::abs((*genParticle)->pdgId);
		nDecayProductsPerType[pdgId] = SafeMap::GetWithDefault(nDecayProductsPerType, pdgId, 0) + 1;
		++iDaughter;
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
}
