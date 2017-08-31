#pragma once

#include <TDirectory.h>
#include <TTree.h>

#include "Artus/Core/interface/ConsumerBase.h"
#include "Artus/Utility/interface/RootFileHelper.h"
#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/**
   \brief KappaCollectionsConsumer, create an TTree with one entry for each valid (Kappa) object.

   This producer creates an ntuple with all data members of the Kappa object as variables.
   Additionally, a matching to gen particles is possible.
   Implementations exist for (Tagged)Jets, Taus, Muons and Electrons.
   The respective producer for valid objects has to be run before.

   This Consumer needs the following config tags:
     BranchGenMatched(Jets|Taus|Muons|Electrons)	(bool, default false) if true: match the valid reco objects to gen objects
     AddGenMatchedParticles							(bool, default true) add a branch with the matching gen particles to the tree
     AddGenMatchedTaus								(bool, default true) add a branch with the matching gen taus to the tree
     AddGenMatchedTauJets							(bool, default true) add a branch with the matching gen tau jets to the tree
*/

template<class TObject, class TObjectMetaInfo>
class KappaCollectionsConsumerBase: public ConsumerBase<KappaTypes>
{

public:

	typedef typename KappaTypes::event_type event_type;
	typedef typename KappaTypes::product_type product_type;
	typedef typename KappaTypes::setting_type setting_type;

	KappaCollectionsConsumerBase(std::string treeName,
	                             std::vector<TObject*> product_type::*validObjects,
	                             bool (setting_type::*GetBranchGenMatchedObjects)(void) const,
	                             TObjectMetaInfo* event_type::*objectMetaInfo = nullptr,
	                             std::map<TObject*, KGenParticle*> product_type::*genParticleMatchedObjects = nullptr,
	                             std::map<TObject*, KGenTau*> product_type::*genTauMatchedObjects = nullptr,
	                             std::map<TObject*, KGenJet*> product_type::*genTauJetMatchedObjects = nullptr) :
		ConsumerBase<KappaTypes>(),
		m_treeName(treeName),
		m_validObjects(validObjects),
		GetBranchGenMatchedObjects(GetBranchGenMatchedObjects),
		m_objectMetaInfo(objectMetaInfo),
		m_objectMetaInfoAvailable(objectMetaInfo != nullptr),
		m_genParticleMatchedObjects(genParticleMatchedObjects),
		m_genParticleMatchedObjectsAvailable(genParticleMatchedObjects != nullptr),
		m_genTauMatchedObjects(genTauMatchedObjects),
		m_genTauMatchedObjectsAvailable(genTauMatchedObjects != nullptr),
		m_genTauJetMatchedObjects(genTauJetMatchedObjects),
		m_genTauJetMatchedObjectsAvailable(genTauJetMatchedObjects != nullptr)
	{
	}

	void Init(setting_type const& settings, metadata_type& metadata) override
	{
		ConsumerBase<KappaTypes>::Init(settings, metadata);

		TDirectory* tmpDirectory = gDirectory;
		RootFileHelper::SafeCd(settings.GetRootOutFile(),
		                       settings.GetRootFileFolder());
		m_tree = new TTree(m_treeName.c_str(), m_treeName.c_str());
		gDirectory = tmpDirectory;
		
		m_tree->Branch("object", &m_currentObject);

		if (m_objectMetaInfoAvailable)
		{
			m_tree->Branch("meta", &m_currentObjectMetaInfo);
		}

		if ((settings.*GetBranchGenMatchedObjects)())
		{
			if (m_genParticleMatchedObjectsAvailable && settings.GetAddGenMatchedParticles())
			{
				m_tree->Branch("genParticle", &m_currentGenParticle);
				m_tree->Branch("genParticleMatched", &m_currentGenParticleMatched, "genParticleMatched/O");
				m_tree->Branch("genParticleMatchedDeltaR", &m_currentGenParticleMatchedDeltaR, "genParticleMatchedDeltaR/D");
			}

			if (m_genTauMatchedObjectsAvailable && settings.GetAddGenMatchedTaus())
			{
				m_tree->Branch("genTau", &m_currentGenTau);
				m_tree->Branch("genTauMatched", &m_currentGenTauMatched, "genTauMatched/O");
				m_tree->Branch("genTauMatchedDeltaR", &m_currentGenTauMatchedDeltaR, "genTauMatchedDeltaR/D");
			}

			if (m_genTauJetMatchedObjectsAvailable && settings.GetAddGenMatchedTauJets())
			{
				m_tree->Branch("genTauJet", &m_currentGenTauJet);
				m_tree->Branch("genTauJetMatched", &m_currentGenTauJetMatched, "genTauJetMatched/O");
				m_tree->Branch("genTauJetMatchedDeltaR", &m_currentGenTauJetMatchedDeltaR, "genTauJetMatchedDeltaR/D");
			}
		}
	}

	void ProcessFilteredEvent(event_type const& event, product_type const& product,
	                          setting_type const& settings, metadata_type const& metadata) override
	{
		for (typename std::vector<TObject*>::const_iterator validObject = (product.*m_validObjects).begin();
		     validObject != (product.*m_validObjects).end(); ++validObject)
		{
			m_currentObject = *(*validObject);
			
			if (m_objectMetaInfoAvailable)
			{
				assert((event.*m_objectMetaInfo));
				m_currentObjectMetaInfo = *(event.*m_objectMetaInfo);
			}
			
			if ((settings.*GetBranchGenMatchedObjects)())
			{
				if (m_genParticleMatchedObjectsAvailable && settings.GetAddGenMatchedParticles())
				{
					KGenParticle* currentGenParticle = SafeMap::GetWithDefault((product.*m_genParticleMatchedObjects), *validObject, static_cast<KGenParticle*>(nullptr));
					m_currentGenParticle = (currentGenParticle != nullptr ? *(static_cast<KGenParticle*>(currentGenParticle)) : KGenParticle());
					m_currentGenParticleMatched = (currentGenParticle != nullptr);
					if (currentGenParticle != nullptr)
					{
						m_currentGenParticleMatchedDeltaR = ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, currentGenParticle->p4);
					}
					else
					{
						m_currentGenParticleMatchedDeltaR = DefaultValues::UndefinedDouble;
					}
				}
				
				if (m_genTauMatchedObjectsAvailable && settings.GetAddGenMatchedTaus())
				{
					KGenTau* currentGenTau = SafeMap::GetWithDefault((product.*m_genTauMatchedObjects), *validObject, static_cast<KGenTau*>(nullptr));
					m_currentGenTau = (currentGenTau != nullptr ? *(static_cast<KGenTau*>(currentGenTau)) : KGenTau());
					m_currentGenTauMatched = (currentGenTau != nullptr);
					if (currentGenTau != nullptr)
					{
						m_currentGenTauMatchedDeltaR = ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, currentGenTau->visible.p4);
					}
					else
					{
						m_currentGenTauMatchedDeltaR = DefaultValues::UndefinedDouble;
					}
				}
				
				if (m_genTauJetMatchedObjectsAvailable && settings.GetAddGenMatchedTauJets())
				{
					KGenJet* currentGenTauJet = SafeMap::GetWithDefault((product.*m_genTauJetMatchedObjects), *validObject, static_cast<KGenJet*>(nullptr));
					m_currentGenTauJet = (currentGenTauJet != nullptr ? *(static_cast<KGenJet*>(currentGenTauJet)) : KGenJet());
					m_currentGenTauJetMatched = (currentGenTauJet != nullptr);
					if (currentGenTauJet != nullptr)
					{
						m_currentGenTauJetMatchedDeltaR = ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, currentGenTauJet->p4);
					}
					else
					{
						m_currentGenTauJetMatchedDeltaR = DefaultValues::UndefinedDouble;
					}
				}
			}
			
			m_tree->Fill();
		}
	}
	
	void Finish(setting_type const& settings, metadata_type const& metadata) override
	{
		RootFileHelper::SafeCd(settings.GetRootOutFile(),
		                       settings.GetRootFileFolder());
		
		m_tree->Write(m_tree->GetName());
	}


private:
	std::string m_treeName;
	std::vector<TObject*> product_type::*m_validObjects;
	bool (setting_type::*GetBranchGenMatchedObjects)(void) const;
	TObjectMetaInfo* event_type::*m_objectMetaInfo;
	bool m_objectMetaInfoAvailable = false;
	std::map<TObject*, KGenParticle*> product_type::*m_genParticleMatchedObjects;
	bool m_genParticleMatchedObjectsAvailable = false;
	std::map<TObject*, KGenTau*> product_type::*m_genTauMatchedObjects;
	bool m_genTauMatchedObjectsAvailable = false;
	std::map<TObject*, KGenJet*> product_type::*m_genTauJetMatchedObjects;
	bool m_genTauJetMatchedObjectsAvailable = false;
	
	TTree* m_tree = nullptr;
	
	TObject m_currentObject;
	TObjectMetaInfo m_currentObjectMetaInfo;
	KGenParticle m_currentGenParticle;
	char m_currentGenParticleMatched;
	double m_currentGenParticleMatchedDeltaR;
	KGenTau m_currentGenTau;
	char m_currentGenTauMatched;
	double m_currentGenTauMatchedDeltaR;
	KGenJet m_currentGenTauJet;
	char m_currentGenTauJetMatched;
	double m_currentGenTauJetMatchedDeltaR;
};



class KappaElectronsConsumer: public KappaCollectionsConsumerBase<KElectron, int>
{

public:
	KappaElectronsConsumer();
	std::string GetConsumerId() const override;
};



class KappaMuonsConsumer: public KappaCollectionsConsumerBase<KMuon, int>
{

public:
	KappaMuonsConsumer();
	std::string GetConsumerId() const override;
};



class KappaTausConsumer: public KappaCollectionsConsumerBase<KTau, KTauMetadata>
{

public:
	KappaTausConsumer();
	std::string GetConsumerId() const override;
};



class KappaJetsConsumer: public KappaCollectionsConsumerBase<KBasicJet, int>
{

public:
	KappaJetsConsumer();
	std::string GetConsumerId() const override;
};



class KappaTaggedJetsConsumer: public KappaCollectionsConsumerBase<KBasicJet, KJetMetadata>
{

public:
	KappaTaggedJetsConsumer();
	std::string GetConsumerId() const override;
};

