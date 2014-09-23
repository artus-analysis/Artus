
#include "Artus/KappaAnalysis/interface/Producers/JetCorrectionsProducer.h"


JetCorrectionsProducer::JetCorrectionsProducer() :
	JetCorrectionsProducerBase<KDataPFJet>(&KappaEvent::m_jets,
	                                               &KappaProduct::m_correctedJets)
{
};

std::string JetCorrectionsProducer::GetProducerId() const {
	return "JetCorrectionsProducer";
}


TaggedJetCorrectionsProducer::TaggedJetCorrectionsProducer() :
	JetCorrectionsProducerBase<KDataPFTaggedJet>(&KappaEvent::m_tjets,
	                                                     &KappaProduct::m_correctedTaggedJets)
{
};

std::string TaggedJetCorrectionsProducer::GetProducerId() const {
	return "TaggedJetCorrectionsProducer";
}

