#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "KappaTools/RootTools/interface/HLTTools.h"

#include "Artus/Core/interface/ProductBase.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"
#include "Artus/KappaAnalysis/interface/Utility/GenParticleDecayTree.h"

/**
   \brief Container class for everything that can be produced in pipeline.

   Defines any outcome that could be produced by a KappaProducer during a common analysis chain in a 
   given KappaPipeline. Via the PipelineRunner the KappaProduct all extra products in the analysis 
   chain will be passed on to subsequent Producers, Filters and Consumers.
*/
class KappaProduct : public ProductBase {
public:
	KappaProduct() {};
	~KappaProduct() {};
	
	// settings to be modified (e.g. in the case of run-dependent settings)
	std::vector<std::string> m_settingsHltPaths;
	
	std::map<size_t, std::vector<std::string> > m_settingsElectronTriggerFiltersByIndex;
	std::map<size_t, std::vector<std::string> > m_settingsMuonTriggerFiltersByIndex;
	std::map<size_t, std::vector<std::string> > m_settingsTauTriggerFiltersByIndex;
	std::map<size_t, std::vector<std::string> > m_settingsJetTriggerFiltersByIndex;
	
	std::map<std::string, std::vector<std::string> > m_settingsElectronTriggerFiltersByHltName;
	std::map<std::string, std::vector<std::string> > m_settingsMuonTriggerFiltersByHltName;
	std::map<std::string, std::vector<std::string> > m_settingsTauTriggerFiltersByHltName;
	std::map<std::string, std::vector<std::string> > m_settingsJetTriggerFiltersByHltName;
	
	std::string m_nickname = "";

	// all weights in this map are multiplied into one "eventWeight" by the EventWeightProducer
	// events in this map can be written out automatically by the KappaLambdaNtupleConsumer
	std::map<std::string, double> m_weights;

	// events in this map can be written out automatically by the KappaLambdaNtupleConsumer
	std::map<std::string, double> m_optionalWeights;
	
	// filled by the GenBosonProducers
	KGenParticle* m_genBosonParticle = nullptr;
	RMFLV m_genBosonLV;
	bool m_genBosonLVFound = false;
	
	std::vector<KGenParticle*> m_genLeptonsFromBosonDecay;
	
	// filled by the ValidGenParticleProducers
	std::vector<KGenParticle*> m_validGenElectrons;
	std::vector<KGenParticle*> m_validGenMuons;
	std::vector<KGenParticle*> m_validGenTaus;

	// filled by the GenTauDecayProducer
	GenParticleDecayTree m_genBosonTree;
	std::map<KGenParticle*, GenParticleDecayTree*> m_genTauDecayTrees;

	/// added by ElectronCorrectionProducer
	// needs to be a shared_ptr in order to be deleted when the product is deleted
	std::vector<std::shared_ptr<KElectron> > m_correctedElectrons;

	/// added by ValidElectronsProducer
	std::vector<KElectron*> m_validElectrons;
	std::vector<KElectron*> m_invalidElectrons;

	/// added by MuonCorrectionProducer
	// needs to be a shared_ptr in order to be deleted when the product is deleted
	std::vector<std::shared_ptr<KMuon> > m_correctedMuons;

	/// added by ValidMuonsProducer
	std::vector<KMuon*> m_validMuons;
	std::vector<KMuon*> m_invalidMuons;
	
	/// added by MuonFSRProducer	
	std::vector<double> m_sumMuonFSRPt;
	std::vector<double> m_MuonPt;

	/// added by TauEnergyCorrectionProducer
	// needs to be a shared_ptr in order to be deleted when the product is deleted
	std::vector<std::shared_ptr<KTau> > m_correctedTaus;
	
	/// added by <Lepton>CorrectionProducers
	std::map<const KLepton*, const KLepton*> m_originalLeptons; // key: corrected, value: original
	
	/// added by ValidTausProducer
	std::vector<KTau*> m_validTaus;
	std::vector<KTau*> m_invalidTaus;
	
	// added by ValidElectronsProducer, ValidMuonsProducer, ValidTausProducer
	std::vector<KLepton*> m_validLeptons;
	std::vector<KLepton*> m_invalidLeptons;

	/// added by JetEnergyCorrectionProducer
	std::vector<std::shared_ptr<KBasicJet> > m_correctedJets;
	std::vector<std::shared_ptr<KJet> > m_correctedTaggedJets;
	std::map<const KBasicJet*, const KBasicJet*> m_originalJets; // key: corrected, value: original
	
	/// added by ValidJetsProducer
	std::vector<KBasicJet*> m_validJets;
	std::vector<KBasicJet*> m_invalidJets;

	/// added by GenParticleProducer
	std::map<int, std::vector<KGenParticle*>> m_genParticlesMap;
	std::vector<KGenParticle*> m_genElectrons;
	std::vector<KGenParticle*> m_genMuons;
	std::vector<KGenParticle*> m_genTaus;

	/// added by GenTauJetProducer
	std::vector<KGenJet*> m_genTauJets;
	
	/// added by GenDiLeptonDecayModeProducer
	KappaEnumTypes::DiLeptonDecayMode m_genDiLeptonDecayMode = KappaEnumTypes::DiLeptonDecayMode::NONE;
	RMFLV m_genDiLeptonBoson;

	/// added by ValidBTaggedJetsProducer
	std::map<std::string,std::vector<KJet*>> m_bTaggedJetsByWp;
	std::map<std::string,std::vector<KJet*>> m_nonBTaggedJetsByWp;
	std::vector<KJet*> m_bTaggedJets;
	std::vector<KJet*> m_nonBTaggedJets;
	
	mutable HLTTools m_hltInfo = HLTTools();
	// selected means fired (and unprescaled if requested)
	std::vector<std::string> m_selectedHltNames;
	std::vector<int> m_selectedHltPositions;
	std::vector<int> m_selectedHltPrescales;

	/// added by TriggerMatchingProducer
	std::map<KElectron*, KLV*> m_triggerMatchedElectrons;
	std::map<KMuon*, KLV*> m_triggerMatchedMuons;
	std::map<KTau*, KLV*> m_triggerMatchedTaus;
	std::map<KBasicJet*, KLV*> m_triggerMatchedJets;
	std::map<KJet*, KLV*> m_triggerMatchedTaggedJets;
	
	std::map<KLepton*, KLV*> m_triggerMatchedLeptons;
	
	/// added by TriggerMatchingProducer
	// m_detailedTriggerMatchedElectrons[reco lepton][HLT name][filter name] = {trigger objects}
	std::map<KElectron*, std::map<std::string, std::map<std::string, std::vector<KLV*> > > > m_detailedTriggerMatchedElectrons;
	std::map<KMuon*, std::map<std::string, std::map<std::string, std::vector<KLV*> > > > m_detailedTriggerMatchedMuons;
	std::map<KTau*, std::map<std::string, std::map<std::string, std::vector<KLV*> > > > m_detailedTriggerMatchedTaus;
	std::map<KBasicJet*, std::map<std::string, std::map<std::string, std::vector<KLV*> > > > m_detailedTriggerMatchedJets;
	std::map<KJet*, std::map<std::string, std::map<std::string, std::vector<KLV*> > > > m_detailedTriggerMatchedTaggedJets;
	
	std::map<KLepton*, std::map<std::string, std::map<std::string, std::vector<KLV*> > >* > m_detailedTriggerMatchedLeptons;

	/// added by GenMatchingProducer
	std::map<KElectron*, KGenParticle*> m_genParticleMatchedElectrons;
	std::map<KMuon*, KGenParticle*> m_genParticleMatchedMuons;
	std::map<KTau*, KGenParticle*> m_genParticleMatchedTaus;
	std::map<KBasicJet*, KGenParticle*> m_genParticleMatchedJets;
	std::map<KLepton*, KGenParticle*> m_genParticleMatchedLeptons;
	float m_ratioGenParticleMatched;
	float m_genParticleMatchDeltaR;

	/// added by GenTauMatchingProducers
	std::map<KElectron*, KGenTau*> m_genTauMatchedElectrons;
	std::map<KMuon*, KGenTau*> m_genTauMatchedMuons;
	std::map<KTau*, KGenTau*> m_genTauMatchedTaus;
	float m_ratioGenTauMatched;
	float m_genTauMatchDeltaR;

	/// added by GenTauJetMatchingProducers
	std::map<KElectron*, KGenJet*> m_genTauJetMatchedElectrons;
	std::map<KMuon*, KGenJet*> m_genTauJetMatchedMuons;
	std::map<KTau*, KGenJet*> m_genTauJetMatchedTaus;

	/// added by ZProducer
	KLV m_z;
	std::pair<KLepton*, KLepton*> m_zLeptons;
	bool m_zValid;

	// MVA outputs
	std::vector<double> m_discriminators;
	
	std::map<const KGenTau*, GenParticleDecayTree::DecayMode> m_genMatchedDecayMode;
	std::map<const KGenTau*, int> m_genMatchedProngSize;
	GenParticleDecayTree::DecayMode m_tau1DecayMode;
	GenParticleDecayTree::DecayMode m_tau2DecayMode;
	int m_tau1ProngSize;
	int m_tau2ProngSize;
	KappaEnumTypes::TauTauDecayMode m_genTauTauDecayMode;

	// GenPartonCounterProducer
	int m_genNPartons = -1;

	// functions to count jets above pt threshold
	template<class TJet>
	static typename std::vector<TJet*>::const_iterator GetLastJetAbovePtThreshold(std::vector<TJet*> const& jets, float lowerPtThreshold)
	{
		typename std::vector<TJet*>::const_iterator lastJetPos = jets.begin();
		for (; lastJetPos != jets.end() && (*lastJetPos)->p4.Pt() > lowerPtThreshold; ++lastJetPos) {}
		return lastJetPos;
	}
	
	template<class TJet>
	static size_t GetNJetsAbovePtThreshold(std::vector<TJet*> const& jets, float lowerPtThreshold)
	{
		return (KappaProduct::GetLastJetAbovePtThreshold(jets, lowerPtThreshold) - jets.begin());
	}
};
