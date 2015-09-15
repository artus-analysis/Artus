
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

void ElectronTriggerMatchingProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                              KappaSettings const& settings) const
{
	TriggerMatchingProducerBase<KElectron>::Produce(event, product, settings);
	
	for (std::map<KElectron*, KLV*>::iterator it = product.m_triggerMatchedElectrons.begin();
	     it != product.m_triggerMatchedElectrons.end(); ++it)
	{
		product.m_triggerMatchedLeptons[&(*(it->first))] = &(*(it->second));
	}
	
	for (std::map<KElectron*, std::map<std::string, std::map<std::string, std::vector<KLV*> > > >::iterator it = product.m_detailedTriggerMatchedElectrons.begin();
	     it != product.m_detailedTriggerMatchedElectrons.end(); ++it)
	{
		product.m_detailedTriggerMatchedLeptons[&(*(it->first))] = &(it->second);
	}
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

void MuonTriggerMatchingProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                              KappaSettings const& settings) const
{
	TriggerMatchingProducerBase<KMuon>::Produce(event, product, settings);
	
	for (std::map<KMuon*, KLV*>::iterator it = product.m_triggerMatchedMuons.begin();
	     it != product.m_triggerMatchedMuons.end(); ++it)
	{
		product.m_triggerMatchedLeptons[&(*(it->first))] = &(*(it->second));
	}
	
	for (std::map<KMuon*, std::map<std::string, std::map<std::string, std::vector<KLV*> > > >::iterator it = product.m_detailedTriggerMatchedMuons.begin();
	     it != product.m_detailedTriggerMatchedMuons.end(); ++it)
	{
		product.m_detailedTriggerMatchedLeptons[&(*(it->first))] = &(it->second);
	}
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

void TauTriggerMatchingProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                              KappaSettings const& settings) const
{
	TriggerMatchingProducerBase<KTau>::Produce(event, product, settings);
	
	for (std::map<KTau*, KLV*>::iterator it = product.m_triggerMatchedTaus.begin();
	     it != product.m_triggerMatchedTaus.end(); ++it)
	{
		product.m_triggerMatchedLeptons[&(*(it->first))] = &(*(it->second));
	}
	
	for (std::map<KTau*, std::map<std::string, std::map<std::string, std::vector<KLV*> > > >::iterator it = product.m_detailedTriggerMatchedTaus.begin();
	     it != product.m_detailedTriggerMatchedTaus.end(); ++it)
	{
		product.m_detailedTriggerMatchedLeptons[&(*(it->first))] = &(it->second);
	}
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

