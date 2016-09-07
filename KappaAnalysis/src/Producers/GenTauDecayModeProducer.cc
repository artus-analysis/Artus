
#include "Artus/KappaAnalysis/interface/Producers/GenTauDecayModeProducer.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/Utility/interface/Utility.h"
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"


std::string GenTauDecayModeProducer::GetProducerId() const {
	return "GenTauDecayModeProducer";
}

void GenTauDecayModeProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);

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
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genTauTauDecayMode", [](KappaEvent const & event, KappaProduct const & product)
	{
		return Utility::ToUnderlyingValue(product.m_genTauTauDecayMode);
	});
}

void GenTauDecayModeProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                      KappaSettings const& settings) const
{
}
