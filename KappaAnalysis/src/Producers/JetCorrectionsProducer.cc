
#include "Artus/KappaAnalysis/interface/Producers/JetCorrectionsProducer.h"


JetCorrectionsProducer::JetCorrectionsProducer() :
	JetCorrectionsProducerBase<KBasicJet>(&event_type::m_basicJets,
	                                      &product_type::m_correctedJets)
{
}

std::string JetCorrectionsProducer::GetProducerId() const {
	return "JetCorrectionsProducer";
}


TaggedJetCorrectionsProducer::TaggedJetCorrectionsProducer() :
	JetCorrectionsProducerBase<KJet>(&event_type::m_tjets,
	                                 &product_type::m_correctedTaggedJets)
{
}

std::string TaggedJetCorrectionsProducer::GetProducerId() const {
	return "TaggedJetCorrectionsProducer";
}

