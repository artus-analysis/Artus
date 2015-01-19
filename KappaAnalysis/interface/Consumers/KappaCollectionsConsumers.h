
#pragma once

#include <TTree.h>

#include "Artus/Core/interface/ConsumerBase.h"
#include "Artus/Utility/interface/RootFileHelper.h"
#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


template<class TObject, class TObjectMetaInfo, class TGenObject>
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
	                             std::map<TObject*, TGenObject*> product_type::*genParticleMatchedObjects = 0) :
		ConsumerBase<KappaTypes>(),
		m_treeName(treeName),
		m_validObjects(validObjects),
		GetBranchGenMatchedObjects(GetBranchGenMatchedObjects),
		m_objectMetaInfo(objectMetaInfo),
		m_objectMetaInfoAvailable(objectMetaInfo != 0),
		m_genParticleMatchedObjects(genParticleMatchedObjects)
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
			m_tree->Branch("genObject", &m_currentGenObject);
			m_tree->Branch("genObjectMatched", &m_currentGenObjectMatched, "genObjectMatched/O");
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
				KGenParticle* current = SafeMap::GetWithDefault((product.*m_genParticleMatchedObjects), *validObject, (KGenParticle*)(0));
				m_currentGenObject = (current != 0 ? *(static_cast<TGenObject*>(current)) : TGenObject());
				m_currentGenObjectMatched = (current != 0);
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
	std::map<TObject*, TGenObject*> product_type::*m_genParticleMatchedObjects;
	
	TTree* m_tree = 0;
	
	TObject m_currentObject;
	TObjectMetaInfo m_currentObjectMetaInfo;
	TGenObject m_currentGenObject;
	char m_currentGenObjectMatched;
};



class KappaElectronsConsumer: public KappaCollectionsConsumerBase<KElectron, int, KGenParticle>
{

public:
	KappaElectronsConsumer();
	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};



class KappaMuonsConsumer: public KappaCollectionsConsumerBase<KMuon, int, KGenParticle>
{

public:
	KappaMuonsConsumer();
	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};



class KappaTausConsumer: public KappaCollectionsConsumerBase<KTau, KTauMetadata, KGenParticle>
{

public:
	KappaTausConsumer();
	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};



class KappaJetsConsumer: public KappaCollectionsConsumerBase<KBasicJet, int, KGenParticle>
{

public:
	KappaJetsConsumer();
	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};


/* TODO
class KappaTaggedJetsConsumer: public KappaCollectionsConsumerBase<KDataPFTaggedJets, KJetMetadata, KGenParticle>
{

public:
	KappaTaggedJetsConsumer();
	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};
*/
