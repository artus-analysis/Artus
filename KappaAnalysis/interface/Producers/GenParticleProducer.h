
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"

/**
   \brief GlobalProducer, to write any available generator particle to the product.


   usage: add to your "genParticles" to your settings
   if you want to use "genParticles", specify also which pdgId you want to have. These particles are then available in the map product.m_genParticlesMap
   with the config parameter genParticleStatus you can specify which status particles have to have to be written to the product. Example for a valid configuration that writes out genTaus as well was Higgs and W with status 3:
   {
   "GetGenParticleTypes" : ["genParticle"],
   "genParticlePdgIds" : [24, 25],
   "genParticleStatus" : 3
   }
*/

class GenParticleProducer: public KappaProducerBase
{
public:

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;

	virtual void Init(KappaSettings const& settings) ARTUS_CPP11_OVERRIDE;
 
	virtual void Produce(KappaEvent const& event, KappaProduct& product,
	                     KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE;


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

