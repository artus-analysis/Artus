#pragma once

#include <TTree.h>

#include "Artus/Core/interface/ConsumerBase.h"
#include "Artus/Utility/interface/RootFileHelper.h"
#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


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

	virtual void Init(setting_type const& settings) override
	{
		ConsumerBase<KappaTypes>::Init(settings);

		RootFileHelper::SafeCd(settings.GetRootOutFile(),
		                       settings.GetRootFileFolder());
		m_tree = new TTree(m_treeName.c_str(), m_treeName.c_str());
		m_tree->Branch("object", &m_currentObject);

		if (m_objectMetaInfoAvailable)
		{
			m_tree->Branch("meta", &m_currentObjectMetaInfo);
		}

		if ((settings.*GetBranchGenMatchedObjects)())
		{
			m_tree->Branch("genParticle", &m_currentGenParticle);
			m_tree->Branch("genParticleMatched", &m_currentGenParticleMatched, "genParticleMatched/O");
			m_tree->Branch("genParticleMatchedDeltaR", &m_currentGenParticleMatchedDeltaR, "genParticleMatchedDeltaR/D");

			m_tree->Branch("genTau", &m_currentGenTau);
			m_tree->Branch("genTauMatched", &m_currentGenTauMatched, "genTauMatched/O");
			m_tree->Branch("genTauMatchedDeltaR", &m_currentGenTauMatchedDeltaR, "genTauMatchedDeltaR/D");

			m_tree->Branch("genTauJet", &m_currentGenTauJet);
			m_tree->Branch("genTauJetMatched", &m_currentGenTauJetMatched, "genTauJetMatched/O");
			m_tree->Branch("genTauJetMatchedDeltaR", &m_currentGenTauJetMatchedDeltaR, "genTauJetMatchedDeltaR/D");
		}
	}

	virtual void ProcessFilteredEvent(event_type const& event, product_type const& product,
	                                  setting_type const& settings) override
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
				if (m_genParticleMatchedObjectsAvailable)
				{
					KGenParticle* currentGenParticle = SafeMap::GetWithDefault((product.*m_genParticleMatchedObjects), *validObject, (KGenParticle*)(nullptr));
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
				
				if (m_genTauMatchedObjectsAvailable)
				{
					KGenTau* currentGenTau = SafeMap::GetWithDefault((product.*m_genTauMatchedObjects), *validObject, (KGenTau*)(nullptr));
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
				
				if (m_genTauJetMatchedObjectsAvailable)
				{
					KGenJet* currentGenTauJet = SafeMap::GetWithDefault((product.*m_genTauJetMatchedObjects), *validObject, (KGenJet*)(nullptr));
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
	
	virtual void Finish(setting_type const& settings) override
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
	virtual std::string GetConsumerId() const override;
};



class KappaMuonsConsumer: public KappaCollectionsConsumerBase<KMuon, int>
{

public:
	KappaMuonsConsumer();
	virtual std::string GetConsumerId() const override;
};



class KappaTausConsumer: public KappaCollectionsConsumerBase<KTau, KTauMetadata>
{

public:
	KappaTausConsumer();
	virtual std::string GetConsumerId() const override;
};



class KappaJetsConsumer: public KappaCollectionsConsumerBase<KBasicJet, int>
{

public:
	KappaJetsConsumer();
	virtual std::string GetConsumerId() const override;
};



class KappaTaggedJetsConsumer: public KappaCollectionsConsumerBase<KBasicJet, KJetMetadata>
{

public:
	KappaTaggedJetsConsumer();
	virtual std::string GetConsumerId() const override;
};

