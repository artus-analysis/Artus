
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"

#include "Artus/KappaAnalysis/interface/KappaProduct.h"

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

template<class TTypes>
class GenParticleProducer: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "gen_particle";
	}

	virtual void Init(setting_type const& settings)
	{
		std::vector<std::string> genParticleNamesToProduce = settings.GetGenParticleTypes();
		for(size_t i = 0; i < genParticleNamesToProduce.size(); ++i)
		{
			genParticleTypesToProduce.push_back(ToGenParticleType(genParticleNamesToProduce[i]));
		}
		std::vector<int> genParticlesPdgIds = settings.GetGenParticlePdgIds();
		for(size_t i = 0; i < genParticlesPdgIds.size(); ++i)
		{
			genParticlesPdgIdsToProduce.push_back(genParticlesPdgIds[i]);
		}
		genParticlesStatus = settings.GetGenParticleStatus();
	}
 
	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const
	{
		if(std::find(genParticleTypesToProduce.begin(), genParticleTypesToProduce.end(), GenParticleType::GENPARTICLE)
		  !=genParticleTypesToProduce.end())
		{
			for (KGenParticles::iterator part = event.m_genParticles->begin();
			    part != event.m_genParticles->end(); ++part)
			{
				if(std::find(genParticlesPdgIdsToProduce.begin(), genParticlesPdgIdsToProduce.end(), part->pdgId())
				  !=genParticlesPdgIdsToProduce.end())
				{
					if((genParticlesStatus == 0) || ( genParticlesStatus == part->status()))
						product.m_genParticlesMap[part->pdgId()].push_back(&(*part));
				}
			}
		}
	}

private:
	std::vector<std::string> genParticles;

	enum class GenParticleType : int
	{
		NONE = -1,
		GENPARTICLE  = 0,
		GENELECTRON = 1,
		GENMUON = 2,
		GENTAU = 3
	};
	static GenParticleType ToGenParticleType(std::string const& genParcticleName)
	{
		if (genParcticleName == "genParticle") return GenParticleType::GENPARTICLE;
		else if (genParcticleName == "genElectron") return GenParticleType::GENELECTRON;
		else if (genParcticleName == "genMuon") return GenParticleType::GENMUON;
		else if (genParcticleName == "genTau") return GenParticleType::GENTAU;
		else return GenParticleType::NONE;
	}
	std::vector<GenParticleType> genParticleTypesToProduce;
	std::vector<int> genParticlesPdgIdsToProduce;
	int genParticlesStatus;

};

