
#include "Artus/KappaAnalysis/interface/Consumers/KappaCollectionsConsumers.h"


KappaElectronsConsumer::KappaElectronsConsumer(std::string treeName,
                                               std::vector<KDataElectron*> product_type::*validObjects) :
	KappaCollectionsConsumerBase(treeName, validObjects)
{
}

std::string KappaElectronsConsumer::GetConsumerId() const
{
	return "KappaElectronsConsumer";
}



KappaMuonsConsumer::KappaMuonsConsumer(std::string treeName,
                                       std::vector<KDataMuon*> product_type::*validObjects) :
	KappaCollectionsConsumerBase(treeName, validObjects)
{
}

std::string KappaMuonsConsumer::GetConsumerId() const
{
	return "KappaMuonsConsumer";
}



KappaTausConsumer::KappaTausConsumer(std::string treeName,
                                     std::vector<KDataPFTau*> product_type::*validObjects,
                                     KTauDiscriminatorMetadata* event_type::*objectMetaInfo) :
	KappaCollectionsConsumerBase(treeName, validObjects, objectMetaInfo)
{
}

std::string KappaTausConsumer::GetConsumerId() const
{
	return "KappaTausConsumer";
}



KappaJetsConsumer::KappaJetsConsumer(std::string treeName,
                                     std::vector<KDataPFJet*> product_type::*validObjects) :
	KappaCollectionsConsumerBase(treeName, validObjects)
{
}

std::string KappaJetsConsumer::GetConsumerId() const
{
	return "KappaJetsConsumer";
}


/* TODO
KappaTaggedJetsConsumer::KappaTaggedJetsConsumer(std::string treeName,
                                                 std::vector<KDataPFTaggedJet*> product_type::*validObjects,
                                                 KTaggerMetadata* event_type::*objectMetaInfo)
{
}

std::string KappaTaggedJetsConsumer::GetConsumerId() const
{
	return "KappaTaggedJetsConsumer";
}
*/
