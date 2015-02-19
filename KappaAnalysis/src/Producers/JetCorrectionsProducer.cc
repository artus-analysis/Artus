
#include "Artus/KappaAnalysis/interface/Producers/JetCorrectionsProducer.h"


JetCorrectionsProducer::JetCorrectionsProducer() :
	JetCorrectionsProducerBase<KBasicJet>(&KappaEvent::m_basicJets,
	                                               &KappaProduct::m_correctedJets)
{
}

std::string JetCorrectionsProducer::GetProducerId() const {
	return "JetCorrectionsProducer";
}


TaggedJetCorrectionsProducer::TaggedJetCorrectionsProducer() :
	JetCorrectionsProducerBase<KJet>(&KappaEvent::m_tjets,
	                                                     &KappaProduct::m_correctedTaggedJets)
{
}

std::string TaggedJetCorrectionsProducer::GetProducerId() const {
	return "TaggedJetCorrectionsProducer";
}

