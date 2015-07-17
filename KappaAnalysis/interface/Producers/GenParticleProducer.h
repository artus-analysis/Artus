
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

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

	virtual std::string GetProducerId() const override;

	virtual void Init(KappaSettings const& settings) override;
 
	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const override;


private:

	enum class GenParticleType : int
	{
		NONE = -1,
		GENPARTICLE  = 0,
		GENELECTRON = 1,
		GENMUON = 2,
		GENTAU = 3
	};
	static GenParticleType ToGenParticleType(std::string const& genParcticleName);
	
	std::vector<GenParticleType> genParticleTypes;

};

