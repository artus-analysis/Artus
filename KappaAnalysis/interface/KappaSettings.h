
#pragma once

#include "Artus/Configuration/interface/SettingsBase.h"
#include "Artus/Configuration/interface/SettingMacros.h"
#include "Artus/Configuration/interface/PropertyTreeSupport.h"

/**
   \brief Reads settings for all parts of the KappaAnalysis code from a prepared
   json configuration file. 

   Defines settings that can be obtained from the json configuration file.
*/

class KappaSettings: public SettingsBase {
public:

	/// name of electron collection in kappa tuple	
	IMPL_SETTING_DEFAULT(std::string, Electrons, "");

	/// name of muon collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, Muons, "");

	/// name of tau collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, Taus, "");
	IMPL_SETTING_DEFAULT(std::string, TauDiscriminatorMetadata, "");
	IMPL_SETTING_DEFAULT(std::string, GenTaus, "");

	/// name of jet collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, Jets, "");

	/// name of tagged jet collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, TaggedJets, "");

	/// name of jet area collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, JetArea, "");

	/// name of MET collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, Met, "");
	
	/// name of GenMET collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, GenMet, "");

	/// names of PF candidate collections in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, PFChargedHadronsPileUp, "");
	IMPL_SETTING_DEFAULT(std::string, PFChargedHadronsNoPileUp, "");
	IMPL_SETTING_DEFAULT(std::string, PFNeutralHadronsNoPileUp, "");
	IMPL_SETTING_DEFAULT(std::string, PFPhotonsNoPileUp, "");

	/// name of beamspot collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, BeamSpot, "");

	/// name of vertexSummary collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, VertexSummary, "");
	
	/// name of trigger infos and objects collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, TriggerInfos, "");
	IMPL_SETTING_DEFAULT(std::string, TriggerObjects, "");

	/// name of genParticle collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, GenParticles, "");

	/// name of lumiMetaData in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, LumiMetadata, ""); // TODO: Default value, move to Artus/Provider

	/// name of genLumiMetaData in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, GenLumiMetadata, ""); // TODO: Default value, move to Artus/Provider

	/// name of eventMetaData in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, EventMetadata, ""); // TODO: Default value, move to Artus/Provider

	/// name of genEventMetaData in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, GenEventMetadata, ""); // TODO: Default value, move to Artus/Provider

	/// name of filterMetaData in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, FilterMetadata, ""); // TODO: Default value, move to Artus/Provider

	/// name of filterSummary in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, FilterSummary, ""); // TODO: Default value, move to Artus/Provider

	/// name of TaggerMetaData in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, TaggerMetadata, ""); // TODO: Default value, move to Artus/Provider


	IMPL_SETTING_DEFAULT(float, XSection, 1.);
	
	IMPL_SETTING_STRINGLIST(JsonFiles);
	IMPL_SETTING_DEFAULT(int, PassRunLow, 1);
	IMPL_SETTING_DEFAULT(int, PassRunHigh, 0);
	
	IMPL_SETTING_INTLIST(RunWhitelist);
	IMPL_SETTING_INTLIST(RunBlacklist);
	IMPL_SETTING_INTLIST(LumiWhitelist);
	IMPL_SETTING_INTLIST(LumiBlacklist);
	IMPL_SETTING_INTLIST(EventWhitelist);
	IMPL_SETTING_INTLIST(EventBlacklist);
	
	IMPL_SETTING_STRINGLIST(HltPaths);
	IMPL_SETTING_DEFAULT(bool, AllowPrescaledTrigger, true);
	
	IMPL_SETTING_STRINGLIST(ElectronTriggerFilterNames);
	IMPL_SETTING_STRINGLIST(MuonTriggerFilterNames);
	IMPL_SETTING_STRINGLIST(TauTriggerFilterNames);
	IMPL_SETTING_STRINGLIST(JetTriggerFilterNames);
	
	IMPL_SETTING_DEFAULT(float, DeltaRTriggerMatchingElectrons, 0.5);
	IMPL_SETTING_DEFAULT(float, DeltaRTriggerMatchingMuons, 0.5);
	IMPL_SETTING_DEFAULT(float, DeltaRTriggerMatchingTaus, 0.5);
	IMPL_SETTING_DEFAULT(float, DeltaRTriggerMatchingJets, 0.5);
	
	IMPL_SETTING_DEFAULT(bool, InvalidateNonMatchingElectrons, true);
	IMPL_SETTING_DEFAULT(bool, InvalidateNonMatchingMuons, true);
	IMPL_SETTING_DEFAULT(bool, InvalidateNonMatchingTaus, true);
	IMPL_SETTING_DEFAULT(bool, InvalidateNonMatchingJets, true);
	
	IMPL_SETTING(int, Year);
	
	IMPL_SETTING_DEFAULT(std::string, ValidMuonsInput, "auto");
	IMPL_SETTING(std::string, MuonID);
	IMPL_SETTING(std::string, MuonIsoType);
	IMPL_SETTING(std::string, MuonIso);
	
	IMPL_SETTING_DEFAULT(std::string, ValidElectronsInput, "auto");
	IMPL_SETTING(std::string, ElectronID);
	IMPL_SETTING(std::string, ElectronIsoType);
	IMPL_SETTING(std::string, ElectronIso);
	IMPL_SETTING(std::string, ElectronReco);
	
	IMPL_SETTING_DEFAULT(std::string, ValidTausInput, "auto");
	IMPL_SETTING_STRINGLIST(TauDiscriminators);
	
	IMPL_SETTING_STRINGLIST(JetEnergyCorrectionParameters);
	IMPL_SETTING_DEFAULT(std::string, JetEnergyCorrectionUncertaintyParameters, "");
	IMPL_SETTING_DEFAULT(float, JetEnergyCorrectionUncertaintyShift, 0.0);
	
	IMPL_SETTING_DEFAULT(std::string, ValidJetsInput, "auto");
	IMPL_SETTING(std::string, JetID);
	IMPL_SETTING_DEFAULT(float, JetLeptonLowerDeltaRCut, 0.5);
	IMPL_SETTING_STRINGLIST(PuJetIDs);
	IMPL_SETTING_STRINGLIST(JetTaggerLowerCuts);
	IMPL_SETTING_STRINGLIST(JetTaggerUpperCuts);
	
	IMPL_SETTING(int, NElectrons);
	IMPL_SETTING(int, NMuons);
	IMPL_SETTING(int, NTaus);
	IMPL_SETTING(int, NJets);
	
	IMPL_SETTING(int, MaxNElectrons);
	IMPL_SETTING(int, MaxNMuons);
	IMPL_SETTING(int, MaxNTaus);
	IMPL_SETTING(int, MaxNJets);
	
	IMPL_SETTING_DEFAULT(size_t, MinNMatchedElectrons, 0);
	IMPL_SETTING_DEFAULT(size_t, MinNMatchedMuons, 0);
	IMPL_SETTING_DEFAULT(size_t, MinNMatchedTaus, 0);
	IMPL_SETTING_DEFAULT(size_t, MinNMatchedJets, 0);
	
	IMPL_SETTING_STRINGLIST(ElectronLowerPtCuts);
	IMPL_SETTING_STRINGLIST(MuonLowerPtCuts);
	IMPL_SETTING_STRINGLIST(TauLowerPtCuts);
	IMPL_SETTING_STRINGLIST(JetLowerPtCuts);
	
	IMPL_SETTING_STRINGLIST(ElectronUpperAbsEtaCuts);
	IMPL_SETTING_STRINGLIST(MuonUpperAbsEtaCuts);
	IMPL_SETTING_STRINGLIST(TauUpperAbsEtaCuts);
	IMPL_SETTING_STRINGLIST(JetUpperAbsEtaCuts);
	
	IMPL_SETTING_DEFAULT(float, BTaggedJetAbsEtaCut, 0.0);
	IMPL_SETTING_DEFAULT(float, BTaggedJetCombinedSecondaryVertexMediumWP, 0.0);
	
	//Reading Boson PdgId for GenTauDecayProducer studies.
	IMPL_SETTING(int, BosonPdgId);

	IMPL_SETTING_STRINGLIST(GenParticleTypes);
	IMPL_SETTING_INTLIST(GenParticlePdgIds);
	IMPL_SETTING_DEFAULT(int, GenParticleStatus, 0);

	IMPL_SETTING(std::string, PileupWeightFile);
	
	IMPL_SETTING(std::string, EventWeight);

	// TMVA reader settings
	IMPL_SETTING_STRINGLIST(TmvaInputQuantities);
	IMPL_SETTING_STRINGLIST(TmvaMethods);
	IMPL_SETTING_STRINGLIST(TmvaWeights);

};
