#include "Artus/KappaAnalysis/interface/Consumers/KappaCollectionsConsumers.h"


KappaElectronsConsumer::KappaElectronsConsumer() :
	KappaCollectionsConsumerBase("electrons",
	                             &product_type::m_validElectrons,
	                             &setting_type::GetBranchGenMatchedElectrons,
	                             ARTUS_CPP11_NULLPTR,
	                             &product_type::m_genParticleMatchedElectrons,
	                             &product_type::m_genTauMatchedElectrons,
	                             &product_type::m_genTauJetMatchedElectrons)
{
}

std::string KappaElectronsConsumer::GetConsumerId() const
{
	return "KappaElectronsConsumer";
}



KappaMuonsConsumer::KappaMuonsConsumer() :
	KappaCollectionsConsumerBase("muons",
	                             &product_type::m_validMuons,
	                             &setting_type::GetBranchGenMatchedMuons,
	                             ARTUS_CPP11_NULLPTR,
	                             &product_type::m_genParticleMatchedMuons,
	                             &product_type::m_genTauMatchedMuons,
	                             &product_type::m_genTauJetMatchedMuons)
{
}

std::string KappaMuonsConsumer::GetConsumerId() const
{
	return "KappaMuonsConsumer";
}



KappaTausConsumer::KappaTausConsumer() :
	KappaCollectionsConsumerBase("taus",
	                             &product_type::m_validTaus,
	                             &setting_type::GetBranchGenMatchedTaus,
	                             &event_type::m_tauMetadata,
	                             &product_type::m_genParticleMatchedTaus,
	                             &product_type::m_genTauMatchedTaus,
	                             &product_type::m_genTauJetMatchedTaus)
{
}

std::string KappaTausConsumer::GetConsumerId() const
{
	return "KappaTausConsumer";
}



KappaJetsConsumer::KappaJetsConsumer() :
	KappaCollectionsConsumerBase("jets",
	                             &product_type::m_validJets,
	                             &setting_type::GetBranchGenMatchedJets,
	                             ARTUS_CPP11_NULLPTR,
	                             &product_type::m_genParticleMatchedJets,
	                             ARTUS_CPP11_NULLPTR,
	                             ARTUS_CPP11_NULLPTR)
{
}

std::string KappaJetsConsumer::GetConsumerId() const
{
	return "KappaJetsConsumer";
}



KappaTaggedJetsConsumer::KappaTaggedJetsConsumer() :
	KappaCollectionsConsumerBase("tjets",
	                             &product_type::m_validJets,
	                             &setting_type::GetBranchGenMatchedJets,
	                             &event_type::m_jetMetadata,
	                             &product_type::m_genParticleMatchedJets,
	                             ARTUS_CPP11_NULLPTR,
	                             ARTUS_CPP11_NULLPTR)
{
}

std::string KappaTaggedJetsConsumer::GetConsumerId() const
{
	return "KappaTaggedJetsConsumer";
}
