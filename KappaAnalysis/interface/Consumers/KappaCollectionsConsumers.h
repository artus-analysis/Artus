#pragma once

#include <TTree.h>

#include "Artus/Core/interface/ConsumerBase.h"
#include "Artus/Utility/interface/RootFileHelper.h"
#include "Artus/Utility/interface/SafeMap.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "TLorentzVector.h"

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
	                             bool (setting_type::*GetBranchObjectMatchedPF)(void) const,
	                             bool (setting_type::*GetBranchObjectMatchedPFEtsum)(void) const,
	                             TObjectMetaInfo* event_type::*objectMetaInfo = nullptr,
	                             std::map<TObject*, KGenParticle*> product_type::*genParticleMatchedObjects = nullptr,
	                             std::map<TObject*, KGenTau*> product_type::*genTauMatchedObjects = nullptr,
	                             std::map<TObject*, KGenJet*> product_type::*genTauJetMatchedObjects = nullptr) :
		ConsumerBase<KappaTypes>(),
		m_treeName(treeName),
		m_validObjects(validObjects),
		GetBranchGenMatchedObjects(GetBranchGenMatchedObjects),
		GetBranchObjectMatchedPF(GetBranchObjectMatchedPF),
		GetBranchObjectMatchedPFEtsum(GetBranchObjectMatchedPFEtsum),
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
		
		if ((settings.*GetBranchObjectMatchedPFEtsum)())
		{
			m_tree->Branch("pfPhotonsNoPileUpEtsum_001", &m_pfPhotonsNoPileUpEtsum_001);
			m_tree->Branch("pfPhotonsNoPileUpEtsum_0025", &m_pfPhotonsNoPileUpEtsum_0025);
			m_tree->Branch("pfPhotonsNoPileUpEtsum_005", &m_pfPhotonsNoPileUpEtsum_005);
			m_tree->Branch("pfPhotonsNoPileUpEtsum_01", &m_pfPhotonsNoPileUpEtsum_01);
		}
		
		if ((settings.*GetBranchObjectMatchedPF)())
		{
			m_tree->Branch("pfAllChargedParticlesNoPileupDeltaR", &m_pfAllChargedParticlesNoPileupDeltaR);
			m_tree->Branch("pfAllChargedParticlesNoPileupPt", &m_pfAllChargedParticlesNoPileupPt);
			m_tree->Branch("pfChargedHadronsPileUpDeltaR", &m_pfChargedHadronsPileUpDeltaR);
			m_tree->Branch("pfChargedHadronsPileUpPt", &m_pfChargedHadronsPileUpPt);
			m_tree->Branch("pfChargedHadronsNoPileUpDeltaR", &m_pfChargedHadronsNoPileUpDeltaR);
			m_tree->Branch("pfChargedHadronsNoPileUpPt", &m_pfChargedHadronsNoPileUpPt);
			m_tree->Branch("pfNeutralHadronsNoPileUpDeltaR", &m_pfNeutralHadronsNoPileUpDeltaR);
			m_tree->Branch("pfNeutralHadronsNoPileUpEt", &m_pfNeutralHadronsNoPileUpEt);
			m_tree->Branch("pfPhotonsNoPileUpDeltaR", &m_pfPhotonsNoPileUpDeltaR);
			m_tree->Branch("pfPhotonsNoPileUpEt", &m_pfPhotonsNoPileUpEt);
		}
	
		if ((settings.*GetBranchObjectMatchedPFEtsum)())
		{
			m_tree->Branch("pfPhotonsNoPileUpEtsum_001", &m_pfPhotonsNoPileUpEtsum_001);
			m_tree->Branch("pfPhotonsNoPileUpEtsum_0025", &m_pfPhotonsNoPileUpEtsum_0025);
			m_tree->Branch("pfPhotonsNoPileUpEtsum_005", &m_pfPhotonsNoPileUpEtsum_005);
			m_tree->Branch("pfPhotonsNoPileUpEtsum_01", &m_pfPhotonsNoPileUpEtsum_01);
		}
		m_tree->Branch("nPU", &m_nPU);
		m_tree->Branch("nPUMean", &m_nPUMean);
		m_tree->Branch("puWeight", &m_puWeight);

	}

	virtual void ProcessFilteredEvent(event_type const& event, product_type const& product,
	                                  setting_type const& settings) override
	{
		m_pfAllChargedParticlesNoPileupDeltaR.clear();
		m_pfAllChargedParticlesNoPileupPt.clear();
		m_pfChargedHadronsPileUpDeltaR.clear();
		m_pfChargedHadronsPileUpPt.clear();
		m_pfChargedHadronsNoPileUpDeltaR.clear();
		m_pfChargedHadronsNoPileUpPt.clear();
		m_pfNeutralHadronsNoPileUpDeltaR.clear();
		m_pfNeutralHadronsNoPileUpEt.clear();
		m_pfPhotonsNoPileUpDeltaR.clear();
		m_pfPhotonsNoPileUpEt.clear();
		
		m_pfPhotonsNoPileUpEtsum_00001 = 0;
		m_pfPhotonsNoPileUpEtsum_0001 = 0;
		m_pfPhotonsNoPileUpEtsum_001 = 0;
		m_pfPhotonsNoPileUpEtsum_0025 = 0;
		m_pfPhotonsNoPileUpEtsum_005 = 0;
		m_pfPhotonsNoPileUpEtsum_01 = 0;
		
		m_nPU = static_cast<KGenEventInfo*>(event.m_eventInfo)->nPU;
		m_nPUMean = static_cast<KGenEventInfo*>(event.m_eventInfo)->nPUMean;
		//m_puWeight = *((product.m_weights)->puWeight);
		//m_puWeight = *(product.m_weights["puWeight"]);
		m_puWeight = SafeMap::GetWithDefault(product.m_weights, std::string("puWeight"), 1.0);
		
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
				
				if (m_genTauMatchedObjectsAvailable && settings.GetAddGenMatchedTaus())
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
				
				if (m_genTauJetMatchedObjectsAvailable && settings.GetAddGenMatchedTauJets())
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
			if ((settings.*GetBranchObjectMatchedPFEtsum)())
			{
				ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > testvector((*validObject)->p4.Pt(),(*validObject)->p4.Eta(),
					(*validObject)->p4.Phi(),(*validObject)->p4.M());
				for (std::vector<KPFCandidate>::const_iterator pfCandidate = event.m_pfPhotonsNoPileUp->begin();
				pfCandidate != event.m_pfPhotonsNoPileUp->end(); ++pfCandidate)
				{
					//double deltaR = ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, pfCandidate->p4);
					double deltaR = ROOT::Math::VectorUtil::DeltaR(testvector, pfCandidate->p4);
					if (deltaR < 1.2)
					{
						m_pfPhotonsNoPileUpDeltaR.push_back (deltaR);
						m_pfPhotonsNoPileUpEt.push_back (pfCandidate->p4.Pt() );
					}
					if (deltaR < 0.0001)
					{
						m_pfPhotonsNoPileUpEtsum_00001 += pfCandidate->p4.Pt();
					}
					if (deltaR < 0.001)
					{
						m_pfPhotonsNoPileUpEtsum_0001 += pfCandidate->p4.Pt();
					}
					if (deltaR < 0.01)
					{
						m_pfPhotonsNoPileUpEtsum_001 += pfCandidate->p4.Pt();
					}
					if (deltaR < 0.025)
					{
						m_pfPhotonsNoPileUpEtsum_0025 += pfCandidate->p4.Pt();
					}
					if (deltaR < 0.05)
					{
						m_pfPhotonsNoPileUpEtsum_005 += pfCandidate->p4.Pt();
					}
					if (deltaR < 0.1)
					{
						m_pfPhotonsNoPileUpEtsum_01 += pfCandidate->p4.Pt();
					}
				}
			}
			if ((settings.*GetBranchObjectMatchedPF)())
			{
				ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<float> > testvector((*validObject)->p4.Pt(),(*validObject)->p4.Eta(),
					(*validObject)->p4.Phi(),(*validObject)->p4.M());
				bool rotate = false;
				if (rotate)
				{
					testvector.SetPhi(-1*(*validObject)->p4.Phi());
				}
				//testvector = ((*validObject)->p4.Pt);
				//std::cout << "Valid Object p4: " << (*validObject)->p4 << std::endl;
				//std::cout << "Valid Object p4 phi: " << (*validObject)->p4.Phi() << std::endl;
				//std::cout << "Testvector p4: " << testvector << std::endl;
				//std::cout << "Valid Object p4 phi: " << (*validObject)->p4.Phi() << std::endl;
				for (std::vector<KPFCandidate>::const_iterator pfCandidate = event.m_pfAllChargedParticlesNoPileUp->begin();
				pfCandidate != event.m_pfAllChargedParticlesNoPileUp->end(); ++pfCandidate)		
				{
					//double deltaR = ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, pfCandidate->p4);
					double deltaR = ROOT::Math::VectorUtil::DeltaR(testvector, pfCandidate->p4);
					if (deltaR < 1.2)
					{
						m_pfAllChargedParticlesNoPileupDeltaR.push_back (deltaR);
						m_pfAllChargedParticlesNoPileupPt.push_back (pfCandidate->p4.Pt() );
					}
				}
				for (std::vector<KPFCandidate>::const_iterator pfCandidate = event.m_pfChargedHadronsPileUp->begin();
				pfCandidate != event.m_pfChargedHadronsPileUp->end(); ++pfCandidate)
				{
					//double deltaR = ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, pfCandidate->p4);
					double deltaR = ROOT::Math::VectorUtil::DeltaR(testvector, pfCandidate->p4);
					if (deltaR < 1.2)
					{
						m_pfChargedHadronsPileUpDeltaR.push_back (deltaR);
						m_pfChargedHadronsPileUpPt.push_back (pfCandidate->p4.Pt() );
					}
				}
				for (std::vector<KPFCandidate>::const_iterator pfCandidate = event.m_pfChargedHadronsNoPileUp->begin();
				pfCandidate != event.m_pfChargedHadronsNoPileUp->end(); ++pfCandidate)
				{
					//double deltaR = ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, pfCandidate->p4);
					double deltaR = ROOT::Math::VectorUtil::DeltaR(testvector, pfCandidate->p4);
					if (deltaR < 1.2)
					{
						m_pfChargedHadronsNoPileUpDeltaR.push_back (deltaR);
						m_pfChargedHadronsNoPileUpPt.push_back (pfCandidate->p4.Pt() );
					}
				}
				for (std::vector<KPFCandidate>::const_iterator pfCandidate = event.m_pfNeutralHadronsNoPileUp->begin();
				pfCandidate != event.m_pfNeutralHadronsNoPileUp->end(); ++pfCandidate)
				{
					//double deltaR = ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, pfCandidate->p4);
					double deltaR = ROOT::Math::VectorUtil::DeltaR(testvector, pfCandidate->p4);
					if (deltaR < 1.2)
					{
						m_pfNeutralHadronsNoPileUpDeltaR.push_back (deltaR);
						m_pfNeutralHadronsNoPileUpEt.push_back (pfCandidate->p4.Pt() );
					}
				}
				for (std::vector<KPFCandidate>::const_iterator pfCandidate = event.m_pfPhotonsNoPileUp->begin();
				pfCandidate != event.m_pfPhotonsNoPileUp->end(); ++pfCandidate)
				{
					//double deltaR = ROOT::Math::VectorUtil::DeltaR((*validObject)->p4, pfCandidate->p4);
					double deltaR = ROOT::Math::VectorUtil::DeltaR(testvector, pfCandidate->p4);
					if (deltaR < 1.2)
					{
						m_pfPhotonsNoPileUpDeltaR.push_back (deltaR);
						m_pfPhotonsNoPileUpEt.push_back (pfCandidate->p4.Pt() );
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
	bool (setting_type::*GetBranchObjectMatchedPF)(void) const;
	bool (setting_type::*GetBranchObjectMatchedPFEtsum)(void) const;
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

	std::vector<double> m_pfAllChargedParticlesNoPileupDeltaR;
	std::vector<double> m_pfAllChargedParticlesNoPileupPt;
	std::vector<double> m_pfChargedHadronsPileUpDeltaR;
	std::vector<double> m_pfChargedHadronsPileUpPt;
	std::vector<double> m_pfChargedHadronsNoPileUpDeltaR;
	std::vector<double> m_pfChargedHadronsNoPileUpPt;
	std::vector<double> m_pfNeutralHadronsNoPileUpDeltaR;
	std::vector<double> m_pfNeutralHadronsNoPileUpEt;
	std::vector<double> m_pfPhotonsNoPileUpDeltaR;
	std::vector<double> m_pfPhotonsNoPileUpEt;
	double m_pfPhotonsNoPileUpEtsum_00001;
	double m_pfPhotonsNoPileUpEtsum_0001;
	double m_pfPhotonsNoPileUpEtsum_001;
	double m_pfPhotonsNoPileUpEtsum_0025;
	double m_pfPhotonsNoPileUpEtsum_005;
	double m_pfPhotonsNoPileUpEtsum_01;
	double m_nPU;
	double m_nPUMean;
	double m_puWeight;
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

