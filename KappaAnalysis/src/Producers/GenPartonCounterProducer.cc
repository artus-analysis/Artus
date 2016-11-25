
#include "Artus/KappaAnalysis/interface/Producers/GenPartonCounterProducer.h"
#include "Artus/Utility/interface/Utility.h"


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
		if (genParticle->status() != settings.GetPartonStatus())
			continue;

		if (countPartons) {
			// quarks and gluons
			if (std::abs(genParticle->pdgId) == 1 ||
			    std::abs(genParticle->pdgId) == 2 ||
			    std::abs(genParticle->pdgId) == 3 ||
			    std::abs(genParticle->pdgId) == 4 ||
			    std::abs(genParticle->pdgId) == 5 ||
			    std::abs(genParticle->pdgId) == 6 ||
			    std::abs(genParticle->pdgId) == 21)

				++nPartons;
		}

		// start counting partons after finding a boson (for example W or Z)
		if (Utility::Contains(settings.GetBosonPdgIds(), std::abs(genParticle->pdgId)))
		{
			countPartons = true;
		}
	}

	product.m_genNPartons = nPartons;
}
