
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"


/**
   \brief GlobalProducer, for tau decays on generator level.
   
   This producers has the GenParticles as input and will extract the following information from 
   this collection :

   - tree with three generations of decay products : Boson, Bosondaughters, Bosongranddaughters  

   If need arises to store other decay trees, this code can be made more general and
   configurable.
*/

class GenTauDecayProducer: public KappaProducerBase
{
public:
	void Init(KappaSettings const& settings) override;

	void Produce(KappaEvent const& event, KappaProduct& product,
	             KappaSettings const& settings) const override;

private:
	void BuildDecayTree(GenParticleDecayTree& currentDecayTree, KGenParticle* currentGenParticle, event_type const& event) const;
	int BosonPdgId;
	int BosonStatus;
};

