
#include "Artus/KappaAnalysis/interface/Producers/SmearedJetProducer.h"


SmearedJetProducer::SmearedJetProducer() :
  SmearedJetProducerBase<KBasicJet>(&KappaTypes::event_type::m_basicJets,
				    &KappaTypes::product_type::m_correctedJets,
				    &KappaTypes::product_type::m_smearedJets) {}

std::string SmearedJetProducer::GetProducerId() const {
  return "SmearedJetProducer";
}


SmearedTaggedJetProducer::SmearedTaggedJetProducer() :
  SmearedJetProducerBase<KJet>(&KappaTypes::event_type::m_tjets,
			       &KappaTypes::product_type::m_correctedTaggedJets,
			       &KappaTypes::product_type::m_smearedTaggedJets) {}

std::string SmearedTaggedJetProducer::GetProducerId() const {
  return "SmearedTaggedJetProducer";
}
