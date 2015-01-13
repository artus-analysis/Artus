
#include "Artus/KappaAnalysis/interface/Consumers/KappaCollectionsConsumers.h"


KappaElectronsConsumer::KappaElectronsConsumer(std::string treeName,
                                               std::vector<KDataElectron*> product_type::*validObjects,
                                               bool (setting_type::*GetBranchGenMatchedObjects)(void) const,
                                               std::map<KDataElectron*, KGenParticle*> product_type::*genParticleMatchedObjects) :
	KappaCollectionsConsumerBase(treeName, validObjects, GetBranchGenMatchedObjects, genParticleMatchedObjects)
{
}

std::string KappaElectronsConsumer::GetConsumerId() const
{
	return "KappaElectronsConsumer";
}



KappaMuonsConsumer::KappaMuonsConsumer(std::string treeName,
                                       std::vector<KDataMuon*> product_type::*validObjects,
                                       bool (setting_type::*GetBranchGenMatchedObjects)(void) const,
                                       std::map<KDataMuon*, KGenParticle*> product_type::*genParticleMatchedObjects) :
	KappaCollectionsConsumerBase(treeName, validObjects, GetBranchGenMatchedObjects, genParticleMatchedObjects)
{
}

std::string KappaMuonsConsumer::GetConsumerId() const
{
	return "KappaMuonsConsumer";
}



KappaTausConsumer::KappaTausConsumer(std::string treeName,
                                     std::vector<KDataPFTau*> product_type::*validObjects,
                                     bool (setting_type::*GetBranchGenMatchedObjects)(void) const,
                                     std::map<KDataPFTau*, KGenParticle*> product_type::*genParticleMatchedObjects,
                                     KTauDiscriminatorMetadata* event_type::*objectMetaInfo) :
	KappaCollectionsConsumerBase(treeName, validObjects, GetBranchGenMatchedObjects, genParticleMatchedObjects, objectMetaInfo)
{
}

std::string KappaTausConsumer::GetConsumerId() const
{
	return "KappaTausConsumer";
}



KappaJetsConsumer::KappaJetsConsumer(std::string treeName,
                                     std::vector<KDataPFJet*> product_type::*validObjects,
                                     bool (setting_type::*GetBranchGenMatchedObjects)(void) const,
                                     std::map<KDataPFJet*, KGenParticle*> product_type::*genParticleMatchedObjects) :
	KappaCollectionsConsumerBase(treeName, validObjects, GetBranchGenMatchedObjects, genParticleMatchedObjects)
{
}

std::string KappaJetsConsumer::GetConsumerId() const
{
	return "KappaJetsConsumer";
}


/* TODO
KappaTaggedJetsConsumer::KappaTaggedJetsConsumer(std::string treeName,
                                                 std::vector<KDataPFTaggedJet*> product_type::*validObjects,
                                                 bool (setting_type::*GetBranchGenMatchedObjects)(void) const,
                                                 std::map<KDataPFTaggedJet*, KGenParticle*> product_type::*genParticleMatchedObjects,
                                                 KTaggerMetadata* event_type::*objectMetaInfo) :
	KappaCollectionsConsumerBase(treeName, validObjects, GetBranchGenMatchedObjects, genParticleMatchedObjects, objectMetaInfo)
{
}

std::string KappaTaggedJetsConsumer::GetConsumerId() const
{
	return "KappaTaggedJetsConsumer";
}
*/
