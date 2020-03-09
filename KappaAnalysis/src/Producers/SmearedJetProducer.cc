
#include "Artus/KappaAnalysis/interface/Producers/SmearedJetProducer.h"


SmearedJetProducer::SmearedJetProducer() :
  SmearedJetProducerBase<KBasicJet>(&KappaTypes::product_type::m_correctedJets) {}

std::string SmearedJetProducer::GetProducerId() const {
  return "SmearedJetProducer";
}


SmearedTaggedJetProducer::SmearedTaggedJetProducer() :
  SmearedJetProducerBase<KJet>(&KappaTypes::product_type::m_correctedTaggedJets) {}

std::string SmearedTaggedJetProducer::GetProducerId() const {
  return "SmearedTaggedJetProducer";
}
