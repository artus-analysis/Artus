
#include "Artus/KappaAnalysis/interface/Producers/TriggerMatchingProducers.h"


std::string ElectronTriggerMatchingProducer::GetProducerId() const
{
	return "ElectronTriggerMatchingProducer";
}

ElectronTriggerMatchingProducer::ElectronTriggerMatchingProducer() :
	TriggerMatchingProducerBase<KElectron>(&product_type::m_triggerMatchedElectrons,
	                                       &product_type::m_detailedTriggerMatchedElectrons,
	                                       &product_type::m_validElectrons,
	                                       &product_type::m_invalidElectrons,
	                                       &product_type::m_settingsElectronTriggerFiltersByIndex,
	                                       &product_type::m_settingsElectronTriggerFiltersByHltName,
	                                       &setting_type::GetElectronTriggerFilterNames,
	                                       &setting_type::GetDeltaRTriggerMatchingElectrons,
	                                       &setting_type::GetInvalidateNonMatchingElectrons)
{
}

void ElectronTriggerMatchingProducer::Produce(event_type const& event, product_type& product,
                                              setting_type const& settings, metadata_type const& metadata) const
{
	TriggerMatchingProducerBase<KElectron>::Produce(event, product, settings, metadata);

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
	TriggerMatchingProducerBase<KMuon>(&product_type::m_triggerMatchedMuons,
	                                   &product_type::m_detailedTriggerMatchedMuons,
	                                   &product_type::m_validMuons,
	                                   &product_type::m_invalidMuons,
	                                   &product_type::m_settingsMuonTriggerFiltersByIndex,
	                                   &product_type::m_settingsMuonTriggerFiltersByHltName,
	                                   &setting_type::GetMuonTriggerFilterNames,
	                                   &setting_type::GetDeltaRTriggerMatchingMuons,
	                                   &setting_type::GetInvalidateNonMatchingMuons)
{
}

void MuonTriggerMatchingProducer::Produce(event_type const& event, product_type& product,
                                          setting_type const& settings, metadata_type const& metadata) const
{	
	TriggerMatchingProducerBase<KMuon>::Produce(event, product, settings, metadata);

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
	TriggerMatchingProducerBase<KTau>(&product_type::m_triggerMatchedTaus,
	                                  &product_type::m_detailedTriggerMatchedTaus,
	                                  &product_type::m_validTaus,
	                                  &product_type::m_invalidTaus,
	                                  &product_type::m_settingsTauTriggerFiltersByIndex,
	                                  &product_type::m_settingsTauTriggerFiltersByHltName,
	                                  &setting_type::GetTauTriggerFilterNames,
	                                  &setting_type::GetDeltaRTriggerMatchingTaus,
	                                  &setting_type::GetInvalidateNonMatchingTaus)
{
}

void TauTriggerMatchingProducer::Produce(event_type const& event, product_type& product,
                                         setting_type const& settings, metadata_type const& metadata) const
{
	TriggerMatchingProducerBase<KTau>::Produce(event, product, settings, metadata);

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
/*
	if (settings.GetTauTriggerCheckL1Match().size() > 0)
        {
                for (std::map<KTau*, KLV*>::iterator it = product.m_triggerMatchedTaus.begin();
                     it != product.m_triggerMatchedTaus.end(); ++it)
                {
                        float deltaRMax = 0.5;
                        float bestL1Pt = -1.0;
                        bool l1matched = false;
                        for (std::vector<KL1Tau>::iterator l1o = event.m_l1taus->begin(); l1o != event.m_l1taus->end(); ++l1o)
                        {
                                float currentDeltaR = ROOT::Math::VectorUtil::DeltaR((it->first)->p4, l1o->p4);
                                if(currentDeltaR < deltaRMax)
                                {
                                        bestL1Pt = l1o->p4.Pt();
                                        deltaRMax = currentDeltaR;
                                }
                        }
                        if(bestL1Pt >= 32.0) l1matched = true;
                        std::map<std::string, bool> hlt_to_l1;
                        for (std::vector<std::string>::iterator hlt = settings.GetTauTriggerCheckL1Match().begin(); hlt != settings.GetTauTriggerCheckL1Match().end(); ++hlt)
                        {
                                hlt_to_l1[*hlt] = l1matched;
                        }
                        product.m_detailedL1MatchedLeptons[&(*(it->first))] = hlt_to_l1;
                }
	}*/
}


std::string JetTriggerMatchingProducer::GetProducerId() const {
	return "JetTriggerMatchingProducer";
}

JetTriggerMatchingProducer::JetTriggerMatchingProducer() :
	TriggerMatchingProducerBase<KBasicJet>(&product_type::m_triggerMatchedJets,
	                                       &product_type::m_detailedTriggerMatchedJets,
	                                       &product_type::m_validJets,
	                                       &product_type::m_invalidJets,
	                                       &product_type::m_settingsJetTriggerFiltersByIndex,
	                                       &product_type::m_settingsJetTriggerFiltersByHltName,
	                                       &setting_type::GetJetTriggerFilterNames,
	                                       &setting_type::GetDeltaRTriggerMatchingJets,
	                                       &setting_type::GetInvalidateNonMatchingJets)
{
}
