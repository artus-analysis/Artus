
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
	
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZtt", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::TT);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZmt", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::MT);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZet", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::ET);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZee", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::EE);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZmm", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::MM);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZem", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::EM);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genTauTauDecayMode", [](KappaEvent const& event, KappaProduct const& product)
	{
		return Utility::ToUnderlyingValue(product.m_genTauTauDecayMode);
	});
}

void GenDiLeptonDecayModeProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                           KappaSettings const& settings) const
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
