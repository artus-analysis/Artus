
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

	/// added by ValidElectronsProducer
	std::vector<KDataElectron*> m_validElectrons;
	std::vector<KDataElectron*> m_invalidElectrons;

	/// added by ValidMuonsProducer
	std::vector<KDataMuon*> m_validMuons;
	std::vector<KDataMuon*> m_invalidMuons;

	/// added by ValidTausProducer
	std::vector<KDataPFTau*> m_validTaus;
	std::vector<KDataPFTau*> m_invalidTaus;

	/// added by ValidJetsProducer
	std::vector<KDataPFJet*> m_validJets;
	std::vector<KDataPFJet*> m_invalidJets;
	
	/// added by ValidBTaggedJetsProducer
	std::vector<KDataPFTaggedJet*> m_bTaggedJets;
	std::vector<KDataPFTaggedJet*> m_nonBTaggedJets;
	
	HLTTools* m_hltInfo = new HLTTools();
	std::string selectedHltName = "";

	/// added by TriggerMatchingProducer
	std::vector<KDataElectron*> m_triggerMatchedElectrons;
	std::vector<KDataMuon*> m_triggerMatchedMuons;
	std::vector<KDataPFTau*> m_triggerMatchedTaus;
	std::vector<KDataPFJet*> m_triggerMatchedJets;
	std::vector<KDataPFTaggedJet*> m_triggerMatchedTaggedJets;

};
