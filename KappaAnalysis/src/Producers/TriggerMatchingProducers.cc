
#include "Artus/KappaAnalysis/interface/Producers/TriggerMatchingProducers.h"


std::string ElectronTriggerMatchingProducer::GetProducerId() const
{
	return "ElectronTriggerMatchingProducer";
}

ElectronTriggerMatchingProducer::ElectronTriggerMatchingProducer() :
	TriggerMatchingProducerBase<KDataElectron>(&KappaProduct::m_triggerMatchedElectrons,
	                                           &KappaProduct::m_validElectrons,
	                                           &KappaProduct::m_invalidElectrons,
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
	TriggerMatchingProducerBase<KDataMuon>(&KappaProduct::m_triggerMatchedMuons,
	                                       &KappaProduct::m_validMuons,
	                                       &KappaProduct::m_invalidMuons,
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
	TriggerMatchingProducerBase<KDataPFTau>(&KappaProduct::m_triggerMatchedTaus,
	                                        &KappaProduct::m_validTaus,
	                                        &KappaProduct::m_invalidTaus,
	                                        &KappaSettings::GetTauTriggerFilterNames,
	                                                &KappaSettings::GetDeltaRTriggerMatchingTaus,
	                                                &KappaSettings::GetInvalidateNonMatchingTaus)
{
}


std::string JetTriggerMatchingProducer::GetProducerId() const {
	return "JetTriggerMatchingProducer";
}

JetTriggerMatchingProducer::JetTriggerMatchingProducer() :
	TriggerMatchingProducerBase<KDataPFJet>(&KappaProduct::m_triggerMatchedJets,
	                                        &KappaProduct::m_validJets,
	                                        &KappaProduct::m_invalidJets,
	                                        &KappaSettings::GetJetTriggerFilterNames,
	                                                &KappaSettings::GetDeltaRTriggerMatchingJets,
	                                                &KappaSettings::GetInvalidateNonMatchingJets)
{
}

