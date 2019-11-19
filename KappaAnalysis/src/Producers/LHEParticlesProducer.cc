
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/SafeMap.h"

#include "Artus/KappaAnalysis/interface/Producers/LHEParticlesProducer.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"
#include "Artus/Utility/interface/Utility.h"


std::string LHEParticlesProducer::GetProducerId() const {
	return "LHEParticlesProducer";
}

void LHEParticlesProducer::Init(setting_type const& settings, metadata_type& metadata)
{
	ProducerBase<KappaTypes>::Init(settings, metadata);

	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "lheZBosonProductionMode", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return Utility::ToUnderlyingValue(product.m_zBosonProductionMode);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZfromUUbar", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_zBosonProductionMode == KappaEnumTypes::ZBosonProductionMode::U_UBAR);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZfromDDbar", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_zBosonProductionMode == KappaEnumTypes::ZBosonProductionMode::D_DBAR);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZfromCCbar", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_zBosonProductionMode == KappaEnumTypes::ZBosonProductionMode::C_CBAR);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZfromSSbar", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_zBosonProductionMode == KappaEnumTypes::ZBosonProductionMode::S_SBAR);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZfromBBbar", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_zBosonProductionMode == KappaEnumTypes::ZBosonProductionMode::B_BBAR);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "lheDiLeptonDecayMode", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return Utility::ToUnderlyingValue(product.m_lheDiLeptonDecayMode);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZtoEM", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EM);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZtoET", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::ET);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZtoMT", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MT);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZtoEE", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EE);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZtoMM", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MM);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZtoTT", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::TT);
	});

	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity(metadata, "lheZtoLL", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return ((product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::MM) || (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EE) || (product.m_lheDiLeptonDecayMode == KappaEnumTypes::DiLeptonDecayMode::EM));
	});
	
}

void LHEParticlesProducer::Produce(event_type const& event, product_type& product,
                                           setting_type const& settings, metadata_type const& metadata) const
{
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
	
	int pdgIdIn1 = lheParticleIn1->pdgId;
	int pdgIdIn2 = lheParticleIn2->pdgId;
	product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::NONE;
	
	// first try to find quarks in production
	if ((pdgIdIn1 == DefaultValues::pdgIdDown) || (pdgIdIn2 == DefaultValues::pdgIdDown))
	{
		product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::D_DBAR;
	}
	else if ((pdgIdIn1 == DefaultValues::pdgIdUp) || (pdgIdIn2 == DefaultValues::pdgIdUp))
	{
		product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::U_UBAR;
	}
	else if ((pdgIdIn1 == DefaultValues::pdgIdStrange) || (pdgIdIn2 == DefaultValues::pdgIdStrange))
	{
		product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::S_SBAR;
	}
	else if ((pdgIdIn1 == DefaultValues::pdgIdCharm) || (pdgIdIn2 == DefaultValues::pdgIdCharm))
	{
		product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::C_CBAR;
	}
	else if ((pdgIdIn1 == DefaultValues::pdgIdBottom) || (pdgIdIn2 == DefaultValues::pdgIdBottom))
	{
		product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::B_BBAR;
	}
	else if ((pdgIdIn1 == (-1)*DefaultValues::pdgIdDown) || (pdgIdIn2 == (-1)*DefaultValues::pdgIdDown)) // then look for anti-quarks in production
	{
		product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::D_DBAR;
	}
	else if ((pdgIdIn1 == (-1)*DefaultValues::pdgIdUp) || (pdgIdIn2 == (-1)*DefaultValues::pdgIdUp))
	{
		product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::U_UBAR;
	}
	else if ((pdgIdIn1 == (-1)*DefaultValues::pdgIdStrange) || (pdgIdIn2 == (-1)*DefaultValues::pdgIdStrange))
	{
		product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::S_SBAR;
	}
	else if ((pdgIdIn1 == (-1)*DefaultValues::pdgIdCharm) || (pdgIdIn2 == (-1)*DefaultValues::pdgIdCharm))
	{
		product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::C_CBAR;
	}
	else if ((pdgIdIn1 == (-1)*DefaultValues::pdgIdBottom) || (pdgIdIn2 == (-1)*DefaultValues::pdgIdBottom))
	{
		product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::B_BBAR;
	}
	
	// if production more not yet found, take first quark (not anti-quark) as indicator for Z boson production mode
	for (std::vector<KLHEParticle>::iterator lheParticle = event.m_lheParticles->particles.begin();
	     (lheParticle != event.m_lheParticles->particles.end()) && (product.m_zBosonProductionMode == KappaEnumTypes::ZBosonProductionMode::NONE);
	     ++lheParticle)
	{
		int pdgId = lheParticle->pdgId;
		if (pdgId == DefaultValues::pdgIdDown)
		{
			product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::D_DBAR;
		}
		else if (pdgId == DefaultValues::pdgIdUp)
		{
			product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::U_UBAR;
		}
		else if (pdgId == DefaultValues::pdgIdStrange)
		{
			product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::S_SBAR;
		}
		else if (pdgId == DefaultValues::pdgIdCharm)
		{
			product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::C_CBAR;
		}
		else if (pdgId == DefaultValues::pdgIdBottom)
		{
			product.m_zBosonProductionMode = KappaEnumTypes::ZBosonProductionMode::B_BBAR;
		}
	}
	
	// Z boson decay mode
	int absPdgIdOut1 = std::abs(lheParticleOut1->pdgId);
	int absPdgIdOut2 = std::abs(lheParticleOut2->pdgId);
	product.m_lheDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::NONE;
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
