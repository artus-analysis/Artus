
#include "Artus/KappaAnalysis/interface/Producers/GenParticleProducer.h"

std::string GenParticleProducer::GetProducerId() const{
	return "GenParticleProducer";
}

void GenParticleProducer::Init(KappaSettings const& settings)
{
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
	
	if (std::find(genParticleTypes.begin(), genParticleTypes.end(), GenParticleType::GENPARTICLE)
	    != genParticleTypes.end())
	{
		for (KGenParticles::iterator part = event.m_genParticles->begin();
		     part != event.m_genParticles->end(); ++part)
		{
			if (std::find(settings.GetGenParticlePdgIds().begin(), settings.GetGenParticlePdgIds().end(), part->pdgId())
			    != settings.GetGenParticlePdgIds().end())
			{
				if ((settings.GetGenParticleStatus() == 0) || ( settings.GetGenParticleStatus() == part->status()))
				{
					product.m_genParticlesMap[part->pdgId()].push_back(&(*part));
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
