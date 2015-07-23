
#include "Artus/KappaAnalysis/interface/Producers/TriggerMatchingProducers.h"


std::string ElectronTriggerMatchingProducer::GetProducerId() const
{
	return "ElectronTriggerMatchingProducer";
}

ElectronTriggerMatchingProducer::ElectronTriggerMatchingProducer() :
	TriggerMatchingProducerBase<KElectron>(&KappaProduct::m_triggerMatchedElectrons,
	                                       &KappaProduct::m_detailedTriggerMatchedElectrons,
	                                       &KappaProduct::m_validElectrons,
	                                       &KappaProduct::m_invalidElectrons,
	                                       &KappaProduct::m_settingsElectronTriggerFiltersByIndex,
	                                       &KappaProduct::m_settingsElectronTriggerFiltersByHltName,
	                                       &KappaSettings::GetElectronTriggerFilterNames,
	                                       &KappaSettings::GetDeltaRTriggerMatchingElectrons,
	                                       &KappaSettings::GetInvalidateNonMatchingElectrons)
{
}


std::string MuonTriggerMatchingProducer::GetProducerId() const
{
	return "MuonTriggerMatchingProducer";
}

MuonTriggerMatchingProducer::MuonTriggerMatchingProducer() :
	TriggerMatchingProducerBase<KMuon>(&KappaProduct::m_triggerMatchedMuons,
	                                   &KappaProduct::m_detailedTriggerMatchedMuons,
	                                   &KappaProduct::m_validMuons,
	                                   &KappaProduct::m_invalidMuons,
	                                   &KappaProduct::m_settingsMuonTriggerFiltersByIndex,
	                                   &KappaProduct::m_settingsMuonTriggerFiltersByHltName,
	                                   &KappaSettings::GetMuonTriggerFilterNames,
	                                   &KappaSettings::GetDeltaRTriggerMatchingMuons,
	                                   &KappaSettings::GetInvalidateNonMatchingMuons)
{
}


std::string TauTriggerMatchingProducer::GetProducerId() const
{
	return "TauTriggerMatchingProducer";
}

TauTriggerMatchingProducer::TauTriggerMatchingProducer() :
	TriggerMatchingProducerBase<KTau>(&KappaProduct::m_triggerMatchedTaus,
	                                  &KappaProduct::m_detailedTriggerMatchedTaus,
	                                  &KappaProduct::m_validTaus,
	                                  &KappaProduct::m_invalidTaus,
	                                  &KappaProduct::m_settingsTauTriggerFiltersByIndex,
	                                  &KappaProduct::m_settingsTauTriggerFiltersByHltName,
	                                  &KappaSettings::GetTauTriggerFilterNames,
	                                  &KappaSettings::GetDeltaRTriggerMatchingTaus,
	                                  &KappaSettings::GetInvalidateNonMatchingTaus)
{
}


std::string JetTriggerMatchingProducer::GetProducerId() const {
	return "JetTriggerMatchingProducer";
}

JetTriggerMatchingProducer::JetTriggerMatchingProducer() :
	TriggerMatchingProducerBase<KBasicJet>(&KappaProduct::m_triggerMatchedJets,
	                                       &KappaProduct::m_detailedTriggerMatchedJets,
	                                       &KappaProduct::m_validJets,
	                                       &KappaProduct::m_invalidJets,
	                                       &KappaProduct::m_settingsJetTriggerFiltersByIndex,
	                                       &KappaProduct::m_settingsJetTriggerFiltersByHltName,
	                                       &KappaSettings::GetJetTriggerFilterNames,
	                                       &KappaSettings::GetDeltaRTriggerMatchingJets,
	                                       &KappaSettings::GetInvalidateNonMatchingJets)
{
}

