
#include "Artus/KappaAnalysis/interface/Consumers/KappaCollectionsConsumers.h"


KappaElectronsConsumer::KappaElectronsConsumer() :
	KappaCollectionsConsumerBase("electrons",
	                             &product_type::m_validElectrons,
	                             &setting_type::GetBranchGenMatchedElectrons,
	                             0,
	                             &product_type::m_genParticleMatchedElectrons)
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
	                             0,
	                             &product_type::m_genParticleMatchedMuons)
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
	                             &product_type::m_genParticleMatchedTaus)
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
	                             0,
	                             &product_type::m_genParticleMatchedJets)
{
}

std::string KappaJetsConsumer::GetConsumerId() const
{
	return "KappaJetsConsumer";
}


/* TODO
KappaTaggedJetsConsumer::KappaTaggedJetsConsumer() :
	KappaCollectionsConsumerBase("jets",
	                             &product_type::m_validJets,
	                             &setting_type::GetBranchGenMatchedJets,
	                             &event_type::m_jetMetadata,
	                             &product_type::m_genParticleMatchedJets)
{
}

std::string KappaTaggedJetsConsumer::GetConsumerId() const
{
	return "KappaTaggedJetsConsumer";
}
*/
