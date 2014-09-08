
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/KappaAnalysis/interface/KappaProduct.h"


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

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;

	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE;

	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE;
private:
	void BuildDecayTree(MotherDaughterBundle & lastProductParentRef, unsigned int lastEventParentIndex, event_type const& event) const;

};

