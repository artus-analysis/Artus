
#include "Artus/KappaAnalysis/interface/Producers/GenParticleProducer.h"

std::string GenParticleProducer::GetProducerId() const{
	return "GenParticleProducer";
}

void GenParticleProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);
	
	genParticleTypes.clear();
	for (std::vector<std::string>::const_iterator genParticleType = settings.GetGenParticleTypes().begin();
	     genParticleType != settings.GetGenParticleTypes().end(); ++genParticleType)
	{
		genParticleTypes.push_back(ToGenParticleType(*genParticleType));
	}
}

void GenParticleProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
{
	assert(event.m_genParticles);

	// gen particles (can be used for quarks, W, Z, .., but also for leptons if needed)
	if (std::find(genParticleTypes.begin(), genParticleTypes.end(), GenParticleType::GENPARTICLE)
	    != genParticleTypes.end())
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
	if (std::find(genParticleTypes.begin(), genParticleTypes.end(), GenParticleType::GENELECTRON)
	    != genParticleTypes.end())
	{
		for (KGenParticles::iterator part = event.m_genParticles->begin();
		     part != event.m_genParticles->end(); ++part)
		{
			if (std::abs(part->pdgId) == 11)
			{
				if ((settings.GetGenElectronStatus() == -1) || ( settings.GetGenElectronStatus() == part->status()))
				{
					product.m_genElectrons.push_back(&(*part));
				}
			}
		}
	}

	// gen muons
	if (std::find(genParticleTypes.begin(), genParticleTypes.end(), GenParticleType::GENMUON)
	    != genParticleTypes.end())
	{
		for (KGenParticles::iterator part = event.m_genParticles->begin();
		     part != event.m_genParticles->end(); ++part)
		{
			if (std::abs(part->pdgId) == 13)
			{
				if ((settings.GetGenMuonStatus() == -1) || ( settings.GetGenMuonStatus() == part->status()))
				{
					product.m_genMuons.push_back(&(*part));
				}
			}
		}
	}

	// gen taus
	if (std::find(genParticleTypes.begin(), genParticleTypes.end(), GenParticleType::GENTAU)
	    != genParticleTypes.end())
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

GenParticleProducer::GenParticleType GenParticleProducer::ToGenParticleType(std::string const& genParcticleName)
{
	if (genParcticleName == "genParticle") return GenParticleType::GENPARTICLE;
	else if (genParcticleName == "genElectron") return GenParticleType::GENELECTRON;
	else if (genParcticleName == "genMuon") return GenParticleType::GENMUON;
	else if (genParcticleName == "genTau") return GenParticleType::GENTAU;
	else return GenParticleType::NONE;
}
