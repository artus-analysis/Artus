
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"

#include "Artus/KappaAnalysis/interface/Producers/GenDiLeptonDecayModeLFVProducer.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"
#include "Artus/Utility/interface/Utility.h"


std::string GenDiLeptonDecayModeLFVProducer::GetProducerId() const {
	return "GenDiLeptonDecayModeLFVProducer";
}

void GenDiLeptonDecayModeLFVProducer::Init(setting_type const& settings, metadata_type& metadata)
{
	ProducerBase<KappaTypes>::Init(settings, metadata);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "isZEM", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genDiLeptonDecayModeLFV == KappaEnumTypes::DiLeptonDecayModeLFV::EM);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "isZET", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genDiLeptonDecayModeLFV == KappaEnumTypes::DiLeptonDecayModeLFV::ET);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "isZMT", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genDiLeptonDecayModeLFV == KappaEnumTypes::DiLeptonDecayModeLFV::MT);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "genDiLeptonDecayModeLFV", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return Utility::ToUnderlyingValue(product.m_genDiLeptonDecayModeLFV);
	});

	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "isZmt", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genTauDecayMode == KappaEnumTypes::TauDecayMode::MT);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "isZet", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genTauDecayMode == KappaEnumTypes::TauDecayMode::ET);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "isZee", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genTauDecayMode == KappaEnumTypes::TauDecayMode::EE);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "isZmm", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genTauDecayMode == KappaEnumTypes::TauDecayMode::MM);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "isZem", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genTauDecayMode == KappaEnumTypes::TauDecayMode::EM);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "genTauDecayMode", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return Utility::ToUnderlyingValue(product.m_genTauDecayMode);
	});
}

void GenDiLeptonDecayModeLFVProducer::Produce(event_type const& event, product_type& product,
                                              setting_type const& settings, metadata_type const& metadata) const
{
	product.m_genDiLeptonDecayModeLFV = KappaEnumTypes::DiLeptonDecayModeLFV::NONE;
	product.m_genTauDecayMode = KappaEnumTypes::TauDecayMode::NONE;

	std::map<int, int> nDecayProductsPerType;
	size_t iDaughter = 0;
	for (std::vector<KGenParticle*>::iterator genParticle = product.m_genLeptonsFromBosonDecay.begin();
	     genParticle != product.m_genLeptonsFromBosonDecay.end() && (iDaughter < 2); ++genParticle)
	{
		int pdgId = std::abs((*genParticle)->pdgId);
		nDecayProductsPerType[pdgId] = SafeMap::GetWithDefault(nDecayProductsPerType, pdgId, 0) + 1;
		++iDaughter;
	}
	
	if ((SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdElectron, 0) == 1) && (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdMuon, 0) == 1))
	{
		product.m_genDiLeptonDecayModeLFV = KappaEnumTypes::DiLeptonDecayModeLFV::EM;
	}
	else if ((SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdElectron, 0) == 1) && (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdTau, 0) == 1))
	{
		product.m_genDiLeptonDecayModeLFV = KappaEnumTypes::DiLeptonDecayModeLFV::ET;

		// check tautau decay modes
		size_t nElectronicDecays = 0;
		size_t nMuonicDecays = 0;
		size_t nHadronicDecays = 0;
		size_t iDaughter = 0;
		for (std::vector<KGenParticle*>::iterator genParticle = product.m_genLeptonsFromBosonDecay.begin();
		     genParticle != product.m_genLeptonsFromBosonDecay.end() && (iDaughter < 2); ++genParticle)
		{
			KGenTau* genTau = SafeMap::GetWithDefault(product.m_validGenTausMap, *genParticle, static_cast<KGenTau*>(nullptr));
			if (genTau)
			{
				if (genTau->isElectronicDecay())
				{
					++nElectronicDecays;
				}
				else if (genTau->isMuonicDecay())
				{
					++nMuonicDecays;
				}
				else if (genTau->isHadronicDecay())
				{
					++nHadronicDecays;
				}
			}
			++iDaughter;
		}

		if (nElectronicDecays == 1)
		{
			if (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdElectron, 0) == 1)
			{
				product.m_genTauDecayMode = KappaEnumTypes::TauDecayMode::EE;
			}
			else if (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdMuon, 0) == 1)
			{
				product.m_genTauDecayMode = KappaEnumTypes::TauDecayMode::EM;
			}
		}
		else if (nMuonicDecays == 1)
		{
			if (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdElectron, 0) == 1)
			{
				product.m_genTauDecayMode = KappaEnumTypes::TauDecayMode::EM;
			}
			else if (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdMuon, 0) == 1)
			{
				product.m_genTauDecayMode = KappaEnumTypes::TauDecayMode::MM;
			}
		}
		else if (nHadronicDecays == 1)
		{
			if (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdElectron, 0) == 1)
			{
				product.m_genTauDecayMode = KappaEnumTypes::TauDecayMode::ET;
			}
			else if (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdMuon, 0) == 1)
			{
				product.m_genTauDecayMode = KappaEnumTypes::TauDecayMode::MT;
			}
		}

	}
	else if ((SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdMuon, 0) == 1) && (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdTau, 0) == 1))
	{
		product.m_genDiLeptonDecayModeLFV = KappaEnumTypes::DiLeptonDecayModeLFV::MT;
		
		// check tautau decay modes
		size_t nElectronicDecays = 0;
		size_t nMuonicDecays = 0;
		size_t nHadronicDecays = 0;
		size_t iDaughter = 0;
		for (std::vector<KGenParticle*>::iterator genParticle = product.m_genLeptonsFromBosonDecay.begin();
		     genParticle != product.m_genLeptonsFromBosonDecay.end() && (iDaughter < 2); ++genParticle)
		{
			KGenTau* genTau = SafeMap::GetWithDefault(product.m_validGenTausMap, *genParticle, static_cast<KGenTau*>(nullptr));
			if (genTau)
			{
				if (genTau->isElectronicDecay())
				{
					++nElectronicDecays;
				}
				else if (genTau->isMuonicDecay())
				{
					++nMuonicDecays;
				}
				else if (genTau->isHadronicDecay())
				{
					++nHadronicDecays;
				}
			}
			++iDaughter;
		}
		
		if (nElectronicDecays == 1)
		{
			if (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdElectron, 0) == 1)
			{
				product.m_genTauDecayMode = KappaEnumTypes::TauDecayMode::EE;
			}
			else if (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdMuon, 0) == 1)
			{
				product.m_genTauDecayMode = KappaEnumTypes::TauDecayMode::EM;
			}
		}
		else if (nMuonicDecays == 1)
		{
			if (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdElectron, 0) == 1)
			{
				product.m_genTauDecayMode = KappaEnumTypes::TauDecayMode::EM;
			}
			else if (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdMuon, 0) == 1)
			{
				product.m_genTauDecayMode = KappaEnumTypes::TauDecayMode::MM;
			}
		}
		else if (nHadronicDecays == 1)
		{
			if (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdElectron, 0) == 1)
			{
				product.m_genTauDecayMode = KappaEnumTypes::TauDecayMode::ET;
			}
			else if (SafeMap::GetWithDefault(nDecayProductsPerType, DefaultValues::pdgIdMuon, 0) == 1)
			{
				product.m_genTauDecayMode = KappaEnumTypes::TauDecayMode::MT;
			}
		}
	}
}
