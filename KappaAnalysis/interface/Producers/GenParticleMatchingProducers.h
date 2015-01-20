
#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaProducerBase.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Utility/interface/Utility.h"


/** Producer for gen matched jets
 *  Required config tags:
 *  - DeltaRMatchingRecoJetGenParticle (default provided)
 *  - InvalidateNonGenParticleMatchingRecoJets (default provided) 
 *  - InvalidateGenParticleMatchingRecoJets (default provided) 
 *  - JetMatchingAlgorithm (default provided)
 */
class RecoJetGenParticleMatchingProducer: public KappaProducerBase
{

public:

	typedef typename KappaTypes::event_type event_type;
	typedef typename KappaTypes::product_type product_type;
	typedef typename KappaTypes::setting_type setting_type;

	enum class JetMatchingAlgorithm : int
	{
		NONE = 0,
		ALGORITHMIC = 1,
		PHYSIC = 2,
	};
	static JetMatchingAlgorithm ToJetMatchingAlgorithm(std::string const& jetMatchingAlgorithm)
	{
		if (jetMatchingAlgorithm == "algorithmic") return JetMatchingAlgorithm::ALGORITHMIC;
		else if (jetMatchingAlgorithm == "physic") return JetMatchingAlgorithm::PHYSIC;
		else return JetMatchingAlgorithm::NONE;
	}
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE;

	virtual void Produce(event_type const& event, product_type& product,
						 setting_type const& settings) const ARTUS_CPP11_OVERRIDE;


private:
	JetMatchingAlgorithm jetMatchingAlgorithm;

};



/** Abstract Producer class for trigger matching valid leptons
 *
 *	Needs to run after the valid lepton producers.
 */
template<class TLepton>
class RecoLeptonGenParticleMatchingProducerBase: public KappaProducerBase
{

public:

	typedef typename KappaTypes::event_type event_type;
	typedef typename KappaTypes::product_type product_type;
	typedef typename KappaTypes::setting_type setting_type;
	
	RecoLeptonGenParticleMatchingProducerBase(std::map<TLepton*, KGenParticle*> product_type::*genParticleMatchedLeptons,
	                                          std::vector<TLepton*> product_type::*validLeptons,
	                                          std::vector<TLepton*> product_type::*invalidLeptons,
	                                          std::vector<int>& (setting_type::*GetRecoLeptonMatchingGenParticlePdgIds)(void) const,
	                                          float (setting_type::*GetDeltaRMatchingRecoLeptonsGenParticle)(void) const,
	                                          bool (setting_type::*GetInvalidateNonGenParticleMatchingLeptons)(void) const,
	                                          bool (setting_type::*GetInvalidateGenParticleMatchingLeptons)(void) const) :
		m_genParticleMatchedLeptons(genParticleMatchedLeptons),
		m_validLeptons(validLeptons),
		m_invalidLeptons(invalidLeptons),
		GetRecoLeptonMatchingGenParticlePdgIds(GetRecoLeptonMatchingGenParticlePdgIds),
		GetDeltaRMatchingRecoLeptonsGenParticle(GetDeltaRMatchingRecoLeptonsGenParticle),
		GetInvalidateNonGenParticleMatchingLeptons(GetInvalidateNonGenParticleMatchingLeptons),
		GetInvalidateGenParticleMatchingLeptons(GetInvalidateGenParticleMatchingLeptons)
	{
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE 
	{
		KappaProducerBase::Init(settings);
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("ratioGenParticleMatched", [](event_type const & event, product_type const & product)
		{
			return product.m_ratioGenParticleMatched;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("genParticleMatchDeltaR", [](event_type const & event, product_type const & product)
		{
			return product.m_genParticleMatchDeltaR;
		});
	}

	virtual void Produce(event_type const& event, product_type& product,
						 setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		assert(event.m_genParticles);
		
		float ratioGenParticleMatched = 0;
		
		if ((settings.*GetDeltaRMatchingRecoLeptonsGenParticle)() > 0.0)
		{
			// loop over all valid leptons to check
			for (typename std::vector<TLepton*>::iterator validLepton = (product.*m_validLeptons).begin();
				 validLepton != (product.*m_validLeptons).end();)
			{
				bool leptonMatched = false;
				float deltaR = 0.0;
				
				// loop over all genParticles
				for (typename std::vector<KGenParticle>::iterator genParticle = event.m_genParticles->begin();
					 !leptonMatched && genParticle != event.m_genParticles->end(); ++genParticle) 
				{
					// only use genParticles that will decay into comparable particles
					if ((settings.*GetRecoLeptonMatchingGenParticlePdgIds)().empty() ||
					    Utility::Contains((settings.*GetRecoLeptonMatchingGenParticlePdgIds)(), std::abs(genParticle->pdgId())))
					{
						deltaR = ROOT::Math::VectorUtil::DeltaR((*validLepton)->p4, genParticle->p4);
						if(deltaR<(settings.*GetDeltaRMatchingRecoLeptonsGenParticle)())
						{
							(product.*m_genParticleMatchedLeptons)[*validLepton] = &(*genParticle);
							ratioGenParticleMatched += (1.0 / (product.*m_validLeptons).size());
							product.m_genParticleMatchDeltaR = deltaR;
							leptonMatched = true;
							//LOG(INFO) << this->GetProducerId() << " (event " << event.m_eventInfo->nEvent << "): " << (*validLepton)->p4 << " --> " << genParticle->p4 << ", pdg=" << genParticle->pdgId();
						}
						else product.m_genParticleMatchDeltaR = DefaultValues::UndefinedFloat;
					}
					else product.m_genParticleMatchDeltaR = DefaultValues::UndefinedFloat;
				}
				// invalidate (non) matching lepton if requested
				if (((! leptonMatched) && (settings.*GetInvalidateNonGenParticleMatchingLeptons)()) ||
				    (leptonMatched && (settings.*GetInvalidateGenParticleMatchingLeptons)()))
				{
					(product.*m_invalidLeptons).push_back(*validLepton);
					validLepton = (product.*m_validLeptons).erase(validLepton);
				}
				else
				{
					++validLepton;
				}
			}
			// preserve sorting of invalid leptons
			if ((settings.*GetInvalidateNonGenParticleMatchingLeptons)() || (settings.*GetInvalidateGenParticleMatchingLeptons)())
			{
				std::sort((product.*m_invalidLeptons).begin(), (product.*m_invalidLeptons).end(),
						  [](TLepton const* lepton1, TLepton const* lepton2) -> bool
						  { return lepton1->p4.Pt() > lepton2->p4.Pt(); });
			}
		}
		else
		{
			product.m_genParticleMatchDeltaR = DefaultValues::UndefinedFloat;
		}
		
		product.m_ratioGenParticleMatched = ratioGenParticleMatched;
	}


private:
	std::map<TLepton*, KGenParticle*> product_type::*m_genParticleMatchedLeptons; //changed to KGenParticle from const KDataLV
	std::vector<TLepton*> product_type::*m_validLeptons;
	std::vector<TLepton*> product_type::*m_invalidLeptons;
	std::vector<int>& (setting_type::*GetRecoLeptonMatchingGenParticlePdgIds)(void) const;
	float (setting_type::*GetDeltaRMatchingRecoLeptonsGenParticle)(void) const;
	bool (setting_type::*GetInvalidateNonGenParticleMatchingLeptons)(void) const;
	bool (setting_type::*GetInvalidateGenParticleMatchingLeptons)(void) const;
	
	std::map<size_t, std::vector<std::string> > m_leptonTriggerFiltersByIndex;
	std::map<std::string, std::vector<std::string> > m_leptonTriggerFiltersByHltName;

};


/** Producer for gen matched electrons
 *  Required config tags:
 *  - RecoElectronMatchingGenParticlePdgIds (default provided)
 *  - DeltaRMatchingRecoElectronsGenParticle (default provided)
 *  - InvalidateNonGenParticleMatchingRecoElectrons (default provided)
 *  - InvalidateGenParticleMatchingRecoElectrons (default provided)
 */
class RecoElectronGenParticleMatchingProducer: public RecoLeptonGenParticleMatchingProducerBase<KElectron>
{

public:
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;

	RecoElectronGenParticleMatchingProducer();

};


/** Producer for gen matched muons
 *  Required config tags:
 *  - RecoMuonMatchingGenParticlePdgIds (default provided)
 *  - DeltaRMatchingRecoMuonGenParticle (default provided)
 *  - InvalidateNonGenParticleMatchingRecoMuons (default provided)
 *  - InvalidateGenParticleMatchingRecoMuons (default provided)
 */
class RecoMuonGenParticleMatchingProducer: public RecoLeptonGenParticleMatchingProducerBase<KMuon>
{

public:
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;
	
	RecoMuonGenParticleMatchingProducer();

};


/** Producer for gen matched taus
 *  Required config tags:
 *  - RecoTauMatchingGenParticlePdgIds (default provided)
 *  - DeltaRMatchingRecoTauGenParticle (default provided)
 *  - InvalidateNonGenParticleMatchingRecoTaus (default provided)
 *  - InvalidateGenParticleMatchingRecoTaus (default provided)
 */
class RecoTauGenParticleMatchingProducer: public RecoLeptonGenParticleMatchingProducerBase<KTau>
{

public:
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE;
	
	RecoTauGenParticleMatchingProducer();

};

