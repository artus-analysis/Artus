
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
	                             std::map<TObject*, KGenParticle*> product_type::*genMatchedObjects = 0,
	                             TObjectMetaInfo* event_type::*objectMetaInfo = 0) :
		ConsumerBase<KappaTypes>(),
		m_treeName(treeName),
		m_validObjects(validObjects),
		GetBranchGenMatchedObjects(GetBranchGenMatchedObjects),
		m_genMatchedObjects(genMatchedObjects),
		m_objectMetaInfo(objectMetaInfo),
		m_objectMetaInfoAvailable(objectMetaInfo != 0)
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
				KGenParticle* current = SafeMap::GetWithDefault((product.*m_genMatchedObjects), *validObject, (KGenParticle*)(0));
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
	std::map<TObject*, KGenParticle*> product_type::*m_genMatchedObjects;
	TObjectMetaInfo* event_type::*m_objectMetaInfo;
	bool m_objectMetaInfoAvailable = false;
	
	TTree* m_tree = 0;
	
	TObject m_currentObject;
	TObjectMetaInfo m_currentObjectMetaInfo;
	TGenObject m_currentGenObject;
	char m_currentGenObjectMatched;
};



class KappaElectronsConsumer: public KappaCollectionsConsumerBase<KDataElectron, int, KDataGenTau>
{

public:
	KappaElectronsConsumer(std::string treeName = "electrons",
	                       std::vector<KDataElectron*> product_type::*validObjects = &product_type::m_validElectrons,
	                       bool (setting_type::*GetBranchGenMatchedObjects)(void) const = &setting_type::GetBranchGenMatchedElectrons,
	                       std::map<KDataElectron*, KGenParticle*> product_type::*genMatchedObjects = &product_type::m_genMatchedElectrons);

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};



class KappaMuonsConsumer: public KappaCollectionsConsumerBase<KDataMuon, int, KDataGenTau>
{

public:
	KappaMuonsConsumer(std::string treeName = "muons",
	                   std::vector<KDataMuon*> product_type::*validObjects = &product_type::m_validMuons,
	                   bool (setting_type::*GetBranchGenMatchedObjects)(void) const = &setting_type::GetBranchGenMatchedMuons,
	                   std::map<KDataMuon*, KGenParticle*> product_type::*genMatchedObjects = &product_type::m_genMatchedMuons);

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};



class KappaTausConsumer: public KappaCollectionsConsumerBase<KDataPFTau, KTauDiscriminatorMetadata, KDataGenTau>
{

public:
	KappaTausConsumer(std::string treeName = "taus",
	                  std::vector<KDataPFTau*> product_type::*validObjects = &product_type::m_validTaus,
	                  bool (setting_type::*GetBranchGenMatchedObjects)(void) const = &setting_type::GetBranchGenMatchedTaus,
	                  std::map<KDataPFTau*, KGenParticle*> product_type::*genMatchedObjects = &product_type::m_genMatchedTaus,
	                  KTauDiscriminatorMetadata* event_type::*objectMetaInfo = &event_type::m_tauDiscriminatorMetadata);

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};



class KappaJetsConsumer: public KappaCollectionsConsumerBase<KDataPFJet, int, KGenParticle>
{

public:
	KappaJetsConsumer(std::string treeName = "jets",
	                  std::vector<KDataPFJet*> product_type::*validObjects = &product_type::m_validJets,
	                  bool (setting_type::*GetBranchGenMatchedObjects)(void) const = &setting_type::GetBranchGenMatchedJets,
	                  std::map<KDataPFJet*, KGenParticle*> product_type::*genMatchedObjects = &product_type::m_genMatchedJets);

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};


/* TODO
class KappaTaggedJetsConsumer: public KappaCollectionsConsumerBase<KDataPFTaggedJets, KTaggerMetadata, KGenParticle>
{

public:
	KappaTaggedJetsConsumer(std::string treeName = "jets",
	                        std::vector<KDataPFTaggedJet*> product_type::*validObjects = &product_type::m_validJets,
	                        bool (setting_type::*GetBranchGenMatchedObjects)(void) const = &setting_type::GetBranchGenMatchedJets,
	                        std::map<KDataPFTaggedJet*, KGenParticle*> product_type::*genMatchedObjects = &product_type::m_genMatchedJets,
	                        KTaggerMetadata* event_type::*objectMetaInfo = &event_type::m_taggerMetadata);

	virtual std::string GetConsumerId() const ARTUS_CPP11_OVERRIDE;
};
*/
