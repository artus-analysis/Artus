
#include "Artus/KappaAnalysis/interface/Producers/GenParticleProducer.h"
#include "Artus/Utility/interface/Utility.h"


void GenParticleProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);
	
	m_genParticleTypes.clear();
	for (std::vector<std::string>::const_iterator genParticleType = settings.GetGenParticleTypes().begin();
	     genParticleType != settings.GetGenParticleTypes().end(); ++genParticleType)
	{
		m_genParticleTypes.push_back(KappaEnumTypes::ToGenParticleType(*genParticleType));
	}
}

void GenParticleProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
{
	assert(event.m_genParticles);

	// gen particles (can be used for quarks, W, Z, .., but also for leptons if needed)
	if (Utility::Contains(m_genParticleTypes, KappaEnumTypes::GenParticleType::GENPARTICLE))
	{
		for (KGenParticles::iterator part = event.m_genParticles->begin();
		     part != event.m_genParticles->end(); ++part)
		{
			if (std::find(settings.GetGenParticlePdgIds().begin(), settings.GetGenParticlePdgIds().end(), part->pdgId)
			    != settings.GetGenParticlePdgIds().end())
			{
				if ((settings.GetGenParticleStatus() == -1) || ( settings.GetGenParticleStatus() == part->status()))
				{
					product.m_genParticlesMap[part->pdgId].push_back(&(*part));
				}
			}
		}
	}

	// gen electrons
	if (Utility::Contains(m_genParticleTypes, KappaEnumTypes::GenParticleType::GENELECTRON))
	{
		for (KGenParticles::iterator part = event.m_genParticles->begin();
		     part != event.m_genParticles->end(); ++part)
		{
			if (std::abs(part->pdgId) == 11)
			{
				if ((settings.GetGenElectronStatus() == -1) || ( settings.GetGenElectronStatus() == part->status()))
				{
					if (settings.GetGenElectronFromTauDecay())
					{
						if (part->isDirectPromptTauDecayProduct())
						{
							product.m_genElectrons.push_back(&(*part));
						}
					}
					else
					{
						product.m_genElectrons.push_back(&(*part));
					}
				}
			}
		}
	}

	// gen muons
	if (Utility::Contains(m_genParticleTypes, KappaEnumTypes::GenParticleType::GENMUON))
	{
		for (KGenParticles::iterator part = event.m_genParticles->begin();
		     part != event.m_genParticles->end(); ++part)
		{
			if (std::abs(part->pdgId) == 13)
			{
				if ((settings.GetGenMuonStatus() == -1) || ( settings.GetGenMuonStatus() == part->status()))
				{
					if (settings.GetGenMuonFromTauDecay())
					{
						if (part->isDirectPromptTauDecayProduct())
						{
							product.m_genMuons.push_back(&(*part));
						}
					}
					else
					{
						product.m_genMuons.push_back(&(*part));
					}
				}
			}
		}
	}

	// gen taus
	if (Utility::Contains(m_genParticleTypes, KappaEnumTypes::GenParticleType::GENTAU))
	{
		for (KGenParticles::iterator part = event.m_genParticles->begin();
		     part != event.m_genParticles->end(); ++part)
		{
			if (std::abs(part->pdgId) == 15)
			{
				if ((settings.GetGenTauStatus() == -1) || ( settings.GetGenTauStatus() == part->status()))
				{
					product.m_genTaus.push_back(&(*part));
				}
			}
		}
	}
}

