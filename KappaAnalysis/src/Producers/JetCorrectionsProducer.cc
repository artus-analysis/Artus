
#include "Artus/KappaAnalysis/interface/Producers/JetCorrectionsProducer.h"


JetCorrectionsProducer::JetCorrectionsProducer() :
	JetCorrectionsProducerBase<KBasicJet>(&KappaEvent::m_basicJets,
	                                               &KappaProduct::m_correctedJets)
{
}


TaggedJetCorrectionsProducer::TaggedJetCorrectionsProducer() :
	JetCorrectionsProducerBase<KJet>(&KappaEvent::m_tjets,
	                                                     &KappaProduct::m_correctedTaggedJets)
{
}

