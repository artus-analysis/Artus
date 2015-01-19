
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
	                             TObjectMetaInfo* event_type::*objectMetaInfo = 0,
	                             std::map<TObject*, KGenParticle*> product_type::*genParticleMatchedObjects = 0,
	                             std::map<TObject*, KGenTau*> product_type::*genTauMatchedObjects = 0,
	                             std::map<TObject*, KGenJet*> product_type::*genTauJetMatchedObjects = 0) :
		ConsumerBase<KappaTypes>(),
		m_treeName(treeName),
		m_validObjects(validObjects),
		GetBranchGenMatchedObjects(GetBranchGenMatchedObjects),
		m_objectMetaInfo(objectMetaInfo),
		m_objectMetaInfoAvailable(objectMetaInfo != 0),
		m_genParticleMatchedObjects(genParticleMatchedObjects),
		m_genTauMatchedObjects(genTauMatchedObjects),
		m_genTauJetMatchedObjects(genTauJetMatchedObjects)
	{
	}
	
	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE
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
			
			m_tree->Branch("genTau", &m_currentGenTau);
			m_tree->Branch("genTauMatched", &m_currentGenTauMatched, "genTauMatched/O");
			
			m_tree->Branch("genTauJet", &m_currentGenTauJet);
			m_tree->Branch("genTauJetMatched", &m_currentGenTauJetMatched, "genTauJetMatched/O");
		}
	}
	
	virtual void ProcessFilteredEvent(event_type const& event, product_type const& product,
	                                  setting_type const& settings) ARTUS_CPP11_OVERRIDE
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
				KGenParticle* currentGenParticle = SafeMap::GetWithDefault((product.*m_genParticleMatchedObjects), *validObject, (KGenParticle*)(0));
				m_currentGenParticle = (currentGenParticle != 0 ? *(static_cast<KGenParticle*>(currentGenParticle)) : KGenParticle());
				m_currentGenParticleMatched = (currentGenParticle != 0);
				
				KGenTau* currentGenTau = SafeMap::GetWithDefault((product.*m_genTauMatchedObjects), *validObject, (KGenTau*)(0));
				m_currentGenTau = (currentGenTau != 0 ? *(static_cast<KGenTau*>(currentGenTau)) : KGenTau());
				m_currentGenTauMatched = (currentGenTau != 0);
				
				KGenJet* currentGenTauJet = SafeMap::GetWithDefault((product.*m_genTauJetMatchedObjects), *validObject, (KGenJet*)(0));
				m_currentGenTauJet = (currentGenTauJet != 0 ? *(static_cast<KGenJet*>(currentGenTauJet)) : KGenJet());
				m_currentGenTauJetMatched = (currentGenTauJet != 0);
			}
			
			m_tree->Fill();
		}
	}
	
	virtual void Finish(setting_type const& settings) ARTUS_CPP11_OVERRIDE
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
	std::map<TObject*, KGenTau*> product_type::*m_genTauMatchedObjects;
	std::map<TObject*, KGenJet*> product_type::*m_genTauJetMatchedObjects;
	
	TTree* m_tree = 0;
	
	TObject m_currentObject;
	TObjectMetaInfo m_currentObjectMetaInfo;
	KGenParticle m_currentGenParticle;
	char m_currentGenParticleMatched;
	KGenTau m_currentGenTau;
	char m_currentGenTauMatched;
	KGenJet m_currentGenTauJet;
	char m_currentGenTauJetMatched;
};



class KappaElectronsConsumer: public KappaCollectionsConsumerBase<KElectron, int>
{

public:
	KappaElectronsConsumer();
	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};



class KappaMuonsConsumer: public KappaCollectionsConsumerBase<KMuon, int>
{

public:
	KappaMuonsConsumer();
	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};



class KappaTausConsumer: public KappaCollectionsConsumerBase<KTau, KTauMetadata>
{

public:
	KappaTausConsumer();
	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};



class KappaJetsConsumer: public KappaCollectionsConsumerBase<KBasicJet, int>
{

public:
	KappaJetsConsumer();
	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};


/* TODO
class KappaTaggedJetsConsumer: public KappaCollectionsConsumerBase<KDataPFTaggedJets, KJetMetadata>
{

public:
	KappaTaggedJetsConsumer();
	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};
*/
