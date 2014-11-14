
#include "Artus/KappaAnalysis/interface/Producers/GenPartonCounterProducer.h"


std::string GenPartonCounterProducer::GetProducerId() const {
	return "GenPartonCounterProducer";
}

void GenPartonCounterProducer::Init(KappaSettings const& settings)
{
	ProducerBase<KappaTypes>::Init(settings);
}

void GenPartonCounterProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                       KappaSettings const& settings) const
{
	assert(event.m_genParticles);
	
	int nPartons = 0;
	bool countPartons = false;
	
	for (KGenParticles::const_iterator genParticle = event.m_genParticles->begin();
		 genParticle != event.m_genParticles->end(); ++genParticle)
	{
		if (genParticle->status() != 3) 
			continue;

		if (countPartons) {
			// quarks and gluons
			if (abs(genParticle->pdgId()) == 1 ||
			    abs(genParticle->pdgId()) == 2 ||
			    abs(genParticle->pdgId()) == 3 ||
			    abs(genParticle->pdgId()) == 4 ||
			    abs(genParticle->pdgId()) == 5 ||
			    abs(genParticle->pdgId()) == 6 ||
			    abs(genParticle->pdgId()) == 21)
				
				nPartons++;
		}
		
		// start counting partons after finding a boson (for example W or Z)
		if (abs(genParticle->pdgId()) == settings.GetBosonPdgId()) 
			countPartons = true; 
	}
	
	product.m_genNPartons = nPartons;
}
