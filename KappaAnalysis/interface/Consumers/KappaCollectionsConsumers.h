
#pragma once

#include <TTree.h>

#include "Artus/Core/interface/ConsumerBase.h"
#include "Artus/Utility/interface/RootFileHelper.h"
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
	                             TObjectMetaInfo* event_type::*objectMetaInfo = 0) :
		ConsumerBase<KappaTypes>(),
		m_treeName(treeName),
		m_validObjects(validObjects),
		m_objectMetaInfo(objectMetaInfo)
	{
	}
	
	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE
	{
		ConsumerBase<KappaTypes>::Init(settings);

		RootFileHelper::SafeCd(settings.GetRootOutFile(),
		                       settings.GetRootFileFolder());
		
		m_tree = new TTree(m_treeName.c_str(), m_treeName.c_str());
		
		m_tree->Branch("object", &m_currentObject);
		
		// TODO
		// m_tree->Branch("meta", &m_currentObjectMetaInfo);
	}
	
	virtual void ProcessFilteredEvent(event_type const& event, product_type const& product,
	                                  setting_type const& settings) ARTUS_CPP11_OVERRIDE
	{
		for (typename std::vector<TObject*>::const_iterator validObject = (product.*m_validObjects).begin();
		     validObject != (product.*m_validObjects).end(); ++validObject)
		{
			m_currentObject = *(*validObject);
			
			if ((event.*m_objectMetaInfo) != 0)
			{
				m_currentObjectMetaInfo = *(event.*m_objectMetaInfo);
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
	TObjectMetaInfo* event_type::*m_objectMetaInfo;
	
	TTree* m_tree = 0;
	
	TObject m_currentObject;
	TObjectMetaInfo m_currentObjectMetaInfo;
};



class KappaElectronsConsumer: public KappaCollectionsConsumerBase<KDataElectron, int>
{

public:
	KappaElectronsConsumer(std::string treeName = "electrons",
	                       std::vector<KDataElectron*> product_type::*validObjects = &product_type::m_validElectrons);

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};



class KappaMuonsConsumer: public KappaCollectionsConsumerBase<KDataMuon, int>
{

public:
	KappaMuonsConsumer(std::string treeName = "muons",
	                   std::vector<KDataMuon*> product_type::*validObjects = &product_type::m_validMuons);

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};



class KappaTausConsumer: public KappaCollectionsConsumerBase<KDataPFTau, KTauDiscriminatorMetadata>
{

public:
	KappaTausConsumer(std::string treeName = "taus",
	                  std::vector<KDataPFTau*> product_type::*validObjects = &product_type::m_validTaus,
	                  KTauDiscriminatorMetadata* event_type::*objectMetaInfo = &event_type::m_tauDiscriminatorMetadata);

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};



class KappaJetsConsumer: public KappaCollectionsConsumerBase<KDataPFJet, int>
{

public:
	KappaJetsConsumer(std::string treeName = "jets",
	                  std::vector<KDataPFJet*> product_type::*validObjects = &product_type::m_validJets);

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};


/* TODO
class KappaTaggedJetsConsumer: public KappaCollectionsConsumerBase<KDataPFTaggedJets, KTaggerMetadata>
{

public:
	KappaTaggedJetsConsumer(std::string treeName = "jets",
	                        std::vector<KDataPFTaggedJet*> product_type::*validObjects = &product_type::m_validJets,
	                        KTaggerMetadata* event_type::*objectMetaInfo = &event_type::m_taggerMetadata);

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};
*/
