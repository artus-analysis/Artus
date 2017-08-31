
#include "Artus/KappaAnalysis/interface/Producers/GenPartonCounterProducer.h"
#include "Artus/Utility/interface/Utility.h"


std::string GenPartonCounterProducer::GetProducerId() const {
	return "GenPartonCounterProducer";
}

void GenPartonCounterProducer::Init(setting_type const& settings, metadata_type& metadata)
{
	ProducerBase<KappaTypes>::Init(settings, metadata);
}

void GenPartonCounterProducer::Produce(event_type const& event, product_type& product,
                                       setting_type const& settings, metadata_type const& metadata) const
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
