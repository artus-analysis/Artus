
#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
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

	enum class JetMatchingAlgorithm : int
	{
		NONE = 0,
		ALGORITHMIC = 1,
		PHYSICS = 2,
	};
	static JetMatchingAlgorithm ToJetMatchingAlgorithm(std::string const& jetMatchingAlgorithm);
	
	std::string GetProducerId() const override;

	void Init(setting_type const& settings, metadata_type& metadata) override;

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override;

	KGenParticle* Match(event_type const& event, product_type const& product,
                        setting_type const& settings, KLV* const recoJet) const;

private:
	JetMatchingAlgorithm m_jetMatchingAlgorithm;
	float m_DeltaRMatchingRecoJetGenParticle;
	bool m_InvalidateNonGenParticleMatchingRecoJets;
	bool m_InvalidateGenParticleMatchingRecoJets;
};



/** Abstract Producer class for trigger matching valid leptons
 *
 *	Needs to run after the valid lepton producers.
 */
template<class TLepton>
class RecoLeptonGenParticleMatchingProducerBase: public KappaProducerBase
{

public:

	RecoLeptonGenParticleMatchingProducerBase(std::map<TLepton*, KGenParticle*> product_type::*genParticleMatchedLeptons,
	                                          std::vector<TLepton>* event_type::*leptons,
	                                          std::vector<TLepton*> product_type::*validLeptons,
	                                          std::vector<TLepton*> product_type::*invalidLeptons,
	                                          std::vector<int>& (setting_type::*GetRecoLeptonMatchingGenParticlePdgIds)(void) const,
	                                          int (setting_type::*GetRecoLeptonMatchingGenParticleStatus)(void) const,
	                                          float (setting_type::*GetDeltaRMatchingRecoLeptonsGenParticle)(void) const,
	                                          bool (setting_type::*GetInvalidateNonGenParticleMatchingLeptons)(void) const,
	                                          bool (setting_type::*GetInvalidateGenParticleMatchingLeptons)(void) const,
	                                          bool (setting_type::*GetRecoLeptonMatchingGenParticleMatchAllLeptons)(void) const) :
		m_genParticleMatchedLeptons(genParticleMatchedLeptons),
		m_leptons(leptons),
		m_validLeptons(validLeptons),
		m_invalidLeptons(invalidLeptons),
		GetRecoLeptonMatchingGenParticlePdgIds(GetRecoLeptonMatchingGenParticlePdgIds),
		GetRecoLeptonMatchingGenParticleStatus(GetRecoLeptonMatchingGenParticleStatus),
		GetDeltaRMatchingRecoLeptonsGenParticle(GetDeltaRMatchingRecoLeptonsGenParticle),
		GetInvalidateNonGenParticleMatchingLeptons(GetInvalidateNonGenParticleMatchingLeptons),
		GetInvalidateGenParticleMatchingLeptons(GetInvalidateGenParticleMatchingLeptons),
		GetRecoLeptonMatchingGenParticleMatchAllLeptons(GetRecoLeptonMatchingGenParticleMatchAllLeptons)
	{
	}

	void Init(setting_type const& settings, metadata_type& metadata) override
	{
		KappaProducerBase::Init(settings, metadata);
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "ratioGenParticleMatched", [](event_type const & event, product_type const & product)
		{
			return product.m_ratioGenParticleMatched;
		});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "genParticleMatchDeltaR", [](event_type const & event, product_type const & product)
		{
			return product.m_genParticleMatchDeltaR;
		});
	}

	void Produce(event_type const& event, product_type& product,
	             setting_type const& settings, metadata_type const& metadata) const override
	{
		assert(event.m_genParticles);

		float ratioGenParticleMatched = 0.0f;

		if ((settings.*GetDeltaRMatchingRecoLeptonsGenParticle)() > 0.0f)
		{
			// choose valid leptons or all leptons for matching
			std::vector<TLepton*> leptons;
			if ((settings.*GetRecoLeptonMatchingGenParticleMatchAllLeptons)())
			{
				assert((event.*m_leptons));

				leptons.resize((event.*m_leptons)->size());
				size_t leptonIndex = 0;
				for (typename std::vector<TLepton>::iterator lepton = (event.*m_leptons)->begin(); lepton != (event.*m_leptons)->end(); ++lepton)
				{
					leptons[leptonIndex] = &(*lepton);
					++leptonIndex;
				}
			}
			else
			{
				leptons.resize((product.*m_validLeptons).size());
				size_t leptonIndex = 0;
				for (typename std::vector<TLepton*>::iterator lepton = (product.*m_validLeptons).begin(); lepton != (product.*m_validLeptons).end(); ++lepton)
				{
					leptons[leptonIndex] = *lepton;
					++leptonIndex;
				}
			}
			// loop over all chosen leptons to check
			for (typename std::vector<TLepton*>::iterator lepton = leptons.begin();
				 lepton != leptons.end();)
			{
				bool leptonMatched = false;
				float deltaR = 0.0f;
				float deltaRmin = std::numeric_limits<float>::max();

				// loop over all genParticles
				for (typename std::vector<KGenParticle>::iterator genParticle = event.m_genParticles->begin();
					genParticle != event.m_genParticles->end(); ++genParticle)
				{
					// only use genParticles that will decay into comparable particles
					if ((settings.*GetRecoLeptonMatchingGenParticlePdgIds)().empty() ||
					    Utility::Contains((settings.*GetRecoLeptonMatchingGenParticlePdgIds)(), std::abs(genParticle->pdgId)))
					{
						// only use genParticles with the required status if requested
						if ((settings.*GetRecoLeptonMatchingGenParticleStatus)() == -1 ||
						    (settings.*GetRecoLeptonMatchingGenParticleStatus)() == genParticle->status())
						{
							deltaR = ROOT::Math::VectorUtil::DeltaR((*lepton)->p4, genParticle->p4);
							if(deltaR<(settings.*GetDeltaRMatchingRecoLeptonsGenParticle)() && deltaR<deltaRmin)
							{
								(product.*m_genParticleMatchedLeptons)[*lepton] = &(*genParticle);
								ratioGenParticleMatched += (1.0f / leptons.size());
								product.m_genParticleMatchDeltaR = deltaR;
								deltaRmin = deltaR;
								leptonMatched = true;
								//LOG(INFO) << this->GetProducerId() << " (event " << event.m_eventInfo->nEvent << "): " << (*lepton)->p4 << " --> " << genParticle->p4 << ", pdg=" << genParticle->pdgId << ", status=" << genParticle->status();
							}
							else product.m_genParticleMatchDeltaR = DefaultValues::UndefinedFloat;
						}
						else product.m_genParticleMatchDeltaR = DefaultValues::UndefinedFloat;
					}
					else product.m_genParticleMatchDeltaR = DefaultValues::UndefinedFloat;
				}
				// invalidate (non) matching lepton if requested
				if (!(settings.*GetRecoLeptonMatchingGenParticleMatchAllLeptons)() &&
					(((! leptonMatched) && (settings.*GetInvalidateNonGenParticleMatchingLeptons)()) ||
					(leptonMatched && (settings.*GetInvalidateGenParticleMatchingLeptons)())))
				{
					(product.*m_invalidLeptons).push_back(*lepton);
					lepton = (product.*m_validLeptons).erase(lepton);
				}
				else
				{
					++lepton;
				}
			}
			// preserve sorting of invalid leptons
			if (!(settings.*GetRecoLeptonMatchingGenParticleMatchAllLeptons)() && ((settings.*GetInvalidateNonGenParticleMatchingLeptons)() || (settings.*GetInvalidateGenParticleMatchingLeptons)()))
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
	std::vector<TLepton>* event_type::*m_leptons;
	std::vector<TLepton*> product_type::*m_validLeptons;
	std::vector<TLepton*> product_type::*m_invalidLeptons;
	std::vector<int>& (setting_type::*GetRecoLeptonMatchingGenParticlePdgIds)(void) const;
	int (setting_type::*GetRecoLeptonMatchingGenParticleStatus)(void) const;
	float (setting_type::*GetDeltaRMatchingRecoLeptonsGenParticle)(void) const;
	bool (setting_type::*GetInvalidateNonGenParticleMatchingLeptons)(void) const;
	bool (setting_type::*GetInvalidateGenParticleMatchingLeptons)(void) const;
	bool (setting_type::*GetRecoLeptonMatchingGenParticleMatchAllLeptons)(void) const;
	
	std::map<size_t, std::vector<std::string> > m_leptonTriggerFiltersByIndex;
	std::map<std::string, std::vector<std::string> > m_leptonTriggerFiltersByHltName;

};


/** Producer for gen matched electrons
 *  Required config tags:
 *  - RecoElectronMatchingGenParticlePdgIds (default provided)
 *  - RecoElectronMatchingGenParticleStatus (every status is used by default)
 *  - DeltaRMatchingRecoElectronGenParticle (default provided)
 *  - InvalidateNonGenParticleMatchingRecoElectrons (default provided)
 *  - InvalidateGenParticleMatchingRecoElectrons (default provided)
 */
class RecoElectronGenParticleMatchingProducer: public RecoLeptonGenParticleMatchingProducerBase<KElectron>
{

public:
	
	std::string GetProducerId() const override;

	RecoElectronGenParticleMatchingProducer();

};


/** Producer for gen matched muons
 *  Required config tags:
 *  - RecoMuonMatchingGenParticlePdgIds (default provided)
 *  - RecoMuonMatchingGenParticleStatus (every status is used by default)
 *  - DeltaRMatchingRecoMuonGenParticle (default provided)
 *  - InvalidateNonGenParticleMatchingRecoMuons (default provided)
 *  - InvalidateGenParticleMatchingRecoMuons (default provided)
 */
class RecoMuonGenParticleMatchingProducer: public RecoLeptonGenParticleMatchingProducerBase<KMuon>
{

public:
	
	std::string GetProducerId() const override;
	
	RecoMuonGenParticleMatchingProducer();

};


/** Producer for gen matched taus
 *  Required config tags:
 *  - RecoTauMatchingGenParticlePdgIds (default provided)
 *  - RecoTauMatchingGenParticleStatus (every status is used by default)
 *  - DeltaRMatchingRecoTauGenParticle (default provided)
 *  - InvalidateNonGenParticleMatchingRecoTaus (default provided)
 *  - InvalidateGenParticleMatchingRecoTaus (default provided)
 */
class RecoTauGenParticleMatchingProducer: public RecoLeptonGenParticleMatchingProducerBase<KTau>
{

public:
	
	std::string GetProducerId() const override;
	
	RecoTauGenParticleMatchingProducer();

};

