
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"
#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

/**
   \brief GlobalProducer, to write any available generator particle to the product.

   Gen particles need to be wired with { "GenParticles" : "genParticles" } first.
   "GenParticleTypes" is a list which can contain genParticle for gen particles in general and/or genElectron, genMuon, genTau for the gen leptons.
   If "genParticle" is selected, the pdgIds from the list "GenParticlePdgIds" will be taken as input.
   Example for a valid configuration that writes out genTaus as well as Higgs and W with status 3:
   {
   "GenParticleTypes" : ["genParticle", "genTau"],
   "GenParticlePdgIds" : [24, 25],
   "GenParticleStatus" : 3,
   "GenTauStatus" : 1
   }

   pdgIds can be found here http://pdg.lbl.gov/2002/montecarlorpp.pdf
*/

class GenParticleProducer: public KappaProducerBase
{
public:
	void Init(KappaSettings const& settings) override;
 
	void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;


private:
	
	std::vector<KappaEnumTypes::GenParticleType> m_genParticleTypes;

};

