
#pragma once

#include "Artus/Configuration/interface/SettingsBase.h"
#include "Artus/Configuration/interface/SettingMacros.h"
#include "Artus/Configuration/interface/PropertyTreeSupport.h"

/**
   \brief Reads settings for all parts of the KappaAnalysis code from a prepared
   json configuration file. 

   Defines settings that can be obtained from the json configuration file.
*/

class KappaPipelineSettings: public SettingsBase {
public:

	/// name of electron collection in kappa tuple	
	IMPL_SETTING_DEFAULT(std::string, Electrons, "");

	/// name of muon collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, Muons, "");

	/// name of tau collection in kappa tuple
	IMPL_SETTING_DEFAULT(std::string, Taus, "");
	IMPL_SETTING_DEFAULT(std::string, TauDiscriminatorMetadata, "");

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
	IMPL_SETTING_DEFAULT(std::string, TriggerObjects, "")

	IMPL_SETTING_STRINGLIST(Quantities);
	
	IMPL_SETTING_STRINGLIST(JsonFiles)
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
	
	IMPL_SETTING_DEFAULT(float, DeltaRTriggerMatchingElectron, 0.5);
	IMPL_SETTING_DEFAULT(float, DeltaRTriggerMatchingMuon, 0.5);
	IMPL_SETTING_DEFAULT(float, DeltaRTriggerMatchingTau, 0.5);
	IMPL_SETTING_DEFAULT(float, DeltaRTriggerMatchingJet, 0.5);
	
	IMPL_SETTING(int, Year);
	
	IMPL_SETTING(std::string, MuonID);
	IMPL_SETTING(std::string, MuonIsoType);
	IMPL_SETTING(std::string, MuonIso);
	
	IMPL_SETTING(std::string, ElectronID);
	IMPL_SETTING(std::string, ElectronIsoType);
	IMPL_SETTING(std::string, ElectronIso);
	IMPL_SETTING(std::string, ElectronReco);
	
	IMPL_SETTING_STRINGLIST(TauDiscriminators);

	IMPL_SETTING(std::string, JetID);
	
	IMPL_SETTING(int, NElectrons);
	IMPL_SETTING(int, NMuons);
	IMPL_SETTING(int, NTaus);
	IMPL_SETTING(int, NJets);
	
	IMPL_SETTING_STRINGLIST(ElectronLowerPtCuts);
	IMPL_SETTING_STRINGLIST(MuonLowerPtCuts);
	IMPL_SETTING_STRINGLIST(TauLowerPtCuts);
	IMPL_SETTING_STRINGLIST(JetLowerPtCuts);
	
	IMPL_SETTING_STRINGLIST(ElectronUpperAbsEtaCuts);
	IMPL_SETTING_STRINGLIST(MuonUpperAbsEtaCuts);
	IMPL_SETTING_STRINGLIST(TauUpperAbsEtaCuts);
	IMPL_SETTING_STRINGLIST(JetUpperAbsEtaCuts);
	
	//Reading Boson PdgId for GenTauDecayProducer studies.
	IMPL_SETTING_STRINGLIST(BosonPdgId);

	IMPL_SETTING(std::string, PileupWeightFile);
	
	IMPL_SETTING(std::string, EventWeight);

};
