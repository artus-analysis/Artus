
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
	
	KLHEParticle* lheParticleIn1 = nullptr;
	KLHEParticle* lheParticleIn2 = nullptr;
	KLHEParticle* lheParticleBoson = nullptr;
	KLHEParticle* lheParticleOut1 = nullptr;
	KLHEParticle* lheParticleOut2 = nullptr;
	
	for (std::vector<KLHEParticle>::iterator lheParticle = event.m_lheParticles->particles.begin();
	     lheParticle != event.m_lheParticles->particles.end(); ++lheParticle)
	{
		if ((lheParticle->status == -1) && (lheParticle->p4.Pt() == 0.0))
		{
			if (lheParticleIn1 == nullptr)
			{
				lheParticleIn1 = &(*lheParticle);
			}
			else if (lheParticleIn2 == nullptr)
			{
				lheParticleIn2 = &(*lheParticle);
			}
		}
		else if (lheParticle->status == 2)
		{
			if (lheParticleBoson == nullptr)
			{
				lheParticleBoson = &(*lheParticle);
			}
		}
		else if ((lheParticle->status == 1) &&
		         (std::abs(lheParticle->pdgId) >= DefaultValues::pdgIdElectron) &&
		         (std::abs(lheParticle->pdgId) <= DefaultValues::pdgIdNuTau))
		{
			if (lheParticleOut1 == nullptr)
			{
				lheParticleOut1 = &(*lheParticle);
			}
			else if (lheParticleOut2 == nullptr)
			{
				lheParticleOut2 = &(*lheParticle);
			}
		}
	}
	assert(lheParticleIn1);
	assert(lheParticleIn2);
	assert(lheParticleOut1);
	assert(lheParticleOut2);
	
	int absPdgIdOut1 = std::abs(lheParticleOut1->pdgId);
	int absPdgIdOut2 = std::abs(lheParticleOut2->pdgId);
	if (absPdgIdOut1 == DefaultValues::pdgIdElectron)
	{
		if (absPdgIdOut2 == DefaultValues::pdgIdElectron)
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::EE;
		}
		else if (absPdgIdOut2 == DefaultValues::pdgIdMuon)
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::EM;
		}
		else if (absPdgIdOut2 == DefaultValues::pdgIdTau)
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::ET;
		}
	}
	else if (absPdgIdOut1 == DefaultValues::pdgIdMuon)
	{
		if (absPdgIdOut2 == DefaultValues::pdgIdElectron)
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::EM;
		}
		else if (absPdgIdOut2 == DefaultValues::pdgIdMuon)
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::MM;
		}
		else if (absPdgIdOut2 == DefaultValues::pdgIdTau)
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::MT;
		}
	}
	else if (absPdgIdOut1 == DefaultValues::pdgIdTau)
	{
		if (absPdgIdOut2 == DefaultValues::pdgIdElectron)
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::ET;
		}
		else if (absPdgIdOut2 == DefaultValues::pdgIdMuon)
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::MT;
		}
		else if (absPdgIdOut2 == DefaultValues::pdgIdTau)
		{
			product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::TT;
		}
	}
}
