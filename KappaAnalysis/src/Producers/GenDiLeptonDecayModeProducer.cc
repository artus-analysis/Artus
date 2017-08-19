
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"

#include "Artus/KappaAnalysis/interface/Producers/GenDiLeptonDecayModeProducer.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"
#include "Artus/Utility/interface/Utility.h"


std::string GenDiLeptonDecayModeProducer::GetProducerId() const {
	return "GenDiLeptonDecayModeProducer";
}

void GenDiLeptonDecayModeProducer::Init(setting_type const& settings, metadata_type& metadata)
{
	ProducerBase<KappaTypes>::Init(settings, metadata);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZEE", [](event_type const& event, product_type const& product)
	{
		return (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EE);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZMM", [](event_type const& event, product_type const& product)
	{
		return (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MM);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZTT", [](event_type const& event, product_type const& product)
	{
		return (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::TT);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZLL", [](event_type const& event, product_type const& product)
	{
		return ((product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MM) || (product.m_genDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EE));
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genDiLeptonDecayMode", [](event_type const& event, product_type const& product)
	{
		return Utility::ToUnderlyingValue(product.m_genDiLeptonDecayMode);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZtt", [](event_type const& event, product_type const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::TT);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZmt", [](event_type const& event, product_type const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::MT);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZet", [](event_type const& event, product_type const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::ET);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZee", [](event_type const& event, product_type const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::EE);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZmm", [](event_type const& event, product_type const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::MM);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZem", [](event_type const& event, product_type const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::EM);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genTauTauDecayMode", [](event_type const& event, product_type const& product)
	{
		return Utility::ToUnderlyingValue(product.m_genTauTauDecayMode);
	});
}

void GenDiLeptonDecayModeProducer::Produce(event_type const& event, product_type& product,
                                           setting_type const& settings, metadata_type const& metadata) const
{
	product.m_genDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::NONE;
	product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::NONE;

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
		
		if (nElectronicDecays == 0)
		{
			if ((nMuonicDecays == 0) && (nHadronicDecays == 2))
			{
				product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::TT;
			}
			else if ((nMuonicDecays == 1) && (nHadronicDecays == 1))
			{
				product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::MT;
			}
			else if ((nMuonicDecays == 2) && (nHadronicDecays == 0))
			{
				product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::MM;
			}
		}
		else if (nElectronicDecays == 1)
		{
			if ((nMuonicDecays == 0) && (nHadronicDecays == 1))
			{
				product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::ET;
			}
			else if ((nMuonicDecays == 1) && (nHadronicDecays == 0))
			{
				product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::EM;
			}
		}
		else if ((nElectronicDecays == 2) && (nMuonicDecays == 0) && (nHadronicDecays == 0))
		{
			product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::EE;
		}
	}
}
