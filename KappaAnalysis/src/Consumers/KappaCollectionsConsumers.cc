
#include "Artus/KappaAnalysis/interface/Consumers/KappaCollectionsConsumers.h"


KappaElectronsConsumer::KappaElectronsConsumer(std::string treeName,
                                               std::vector<KElectron*> product_type::*validObjects,
                                               bool (setting_type::*GetBranchGenMatchedObjects)(void) const,
                                               std::map<KElectron*, KGenParticle*> product_type::*genParticleMatchedObjects) :
	KappaCollectionsConsumerBase(treeName, validObjects, GetBranchGenMatchedObjects, genParticleMatchedObjects)
{
}

std::string KappaElectronsConsumer::GetConsumerId() const
{
	return "KappaElectronsConsumer";
}



KappaMuonsConsumer::KappaMuonsConsumer(std::string treeName,
                                       std::vector<KMuon*> product_type::*validObjects,
                                       bool (setting_type::*GetBranchGenMatchedObjects)(void) const,
                                       std::map<KMuon*, KGenParticle*> product_type::*genParticleMatchedObjects) :
	KappaCollectionsConsumerBase(treeName, validObjects, GetBranchGenMatchedObjects, genParticleMatchedObjects)
{
}

std::string KappaMuonsConsumer::GetConsumerId() const
{
	return "KappaMuonsConsumer";
}



KappaTausConsumer::KappaTausConsumer(std::string treeName,
                                     std::vector<KTau*> product_type::*validObjects,
                                     bool (setting_type::*GetBranchGenMatchedObjects)(void) const,
                                     std::map<KTau*, KGenParticle*> product_type::*genParticleMatchedObjects,
                                     KTauMetadata* event_type::*objectMetaInfo) :
	KappaCollectionsConsumerBase(treeName, validObjects, GetBranchGenMatchedObjects, genParticleMatchedObjects, objectMetaInfo)
{
}

std::string KappaTausConsumer::GetConsumerId() const
{
	return "KappaTausConsumer";
}



KappaJetsConsumer::KappaJetsConsumer(std::string treeName,
                                     std::vector<KBasicJet*> product_type::*validObjects,
                                     bool (setting_type::*GetBranchGenMatchedObjects)(void) const,
                                     std::map<KBasicJet*, KGenParticle*> product_type::*genParticleMatchedObjects) :
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
