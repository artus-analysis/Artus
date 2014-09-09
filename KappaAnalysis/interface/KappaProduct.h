
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "KappaTools/RootTools/HLTTools.h"
#include "MotherDaughterBundle.h"

#include "Artus/Core/interface/ProductBase.h"

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

	// all weights are collected in a map
	// and multiplied into one "eventWeight" by the EventWeightProducer
	std::map<std::string, double> m_weights;
	
	std::vector<MotherDaughterBundle> m_genBoson;

	/// added by ElectronCorrectionProducer
	// needs to be a shared_ptr in order to be deleted when the product is deleted
	std::vector<std::shared_ptr<KDataElectron> > m_correctedElectrons;

	/// added by ValidElectronsProducer
	std::vector<KDataElectron*> m_validElectrons;
	std::vector<KDataElectron*> m_invalidElectrons;

	/// added by MuonCorrectionProducer
	// needs to be a shared_ptr in order to be deleted when the product is deleted
	std::vector<std::shared_ptr<KDataMuon> > m_correctedMuons;

	/// added by ValidMuonsProducer
	std::vector<KDataMuon*> m_validMuons;
	std::vector<KDataMuon*> m_invalidMuons;

	/// added by TauEnergyCorrectionProducer
	// needs to be a shared_ptr in order to be deleted when the product is deleted
	std::vector<std::shared_ptr<KDataPFTau> > m_correctedTaus;

	/// added by ValidTausProducer
	std::vector<KDataPFTau*> m_validTaus;
	std::vector<KDataPFTau*> m_invalidTaus;
	
	// added by ValidElectronsProducer, ValidMuonsProducer, ValidTausProducer
	std::vector<KLepton*> m_validLeptons;
	std::vector<KLepton*> m_invalidLeptons;

	/// added by JetEnergyCorrectionProducer
	std::vector<std::shared_ptr<KDataPFJet> > m_correctedJets;
	std::vector<std::shared_ptr<KDataPFTaggedJet> > m_correctedTaggedJets;
	
	/// added by ValidJetsProducer
	std::vector<KDataPFJet*> m_validJets;
	std::vector<KDataPFJet*> m_invalidJets;

	/// added by GenParticleProducer
	std::map<int, std::vector<KGenParticle*>> m_genParticlesMap;

	/// added by ValidBTaggedJetsProducer
	std::vector<KDataPFTaggedJet*> m_bTaggedJets;
	std::vector<KDataPFTaggedJet*> m_nonBTaggedJets;
	
	HLTTools m_hltInfo = HLTTools();
	std::string m_selectedHltName;
	int m_selectedHltPosition;

	/// added by TriggerMatchingProducer
	std::map<KDataElectron*, KDataLV*> m_triggerMatchedElectrons;
	std::map<KDataMuon*, KDataLV*> m_triggerMatchedMuons;
	std::map<KDataPFTau*, KDataLV*> m_triggerMatchedTaus;
	std::map<KDataPFJet*, KDataLV*> m_triggerMatchedJets;
	std::map<KDataPFTaggedJet*, KDataLV*> m_triggerMatchedTaggedJets;
	
	// MVA outputs
	std::vector<double> m_discriminators;

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
