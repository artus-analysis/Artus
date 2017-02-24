
#include "Artus/KappaAnalysis/interface/Producers/ValidJetsProducer.h"


ValidJetsProducer::ValidJetsProducer() : ValidJetsProducerBase<KBasicJet, KBasicJet>(&KappaEvent::m_basicJets,
                                                                            &KappaProduct::m_correctedJets,
                                                                            &KappaProduct::m_validJets)
{
}

ValidTaggedJetsProducer::ValidTaggedJetsProducer() : ValidJetsProducerBase<KJet, KBasicJet>(&KappaEvent::m_tjets,
                                                                                        &KappaProduct::m_correctedTaggedJets,
                                                                                        &KappaProduct::m_validJets)
{
}

void ValidTaggedJetsProducer::Init(KappaSettings const& settings)
{
	ValidJetsProducerBase<KJet, KBasicJet>::Init(settings);
	
	puJetIdsByIndex = Utility::ParseMapTypes<size_t, std::string>(
			Utility::ParseVectorToMap(settings.GetPuJetIDs()),
			puJetIdsByHltName
	);
	
	jetTaggerLowerCutsByTaggerName = Utility::ParseMapTypes<std::string, float>(
			Utility::ParseVectorToMap(settings.GetJetTaggerLowerCuts()),
			jetTaggerLowerCutsByTaggerName
	);
	
	jetTaggerUpperCutsByTaggerName = Utility::ParseMapTypes<std::string, float>(
			Utility::ParseVectorToMap(settings.GetJetTaggerUpperCuts()),
			jetTaggerUpperCutsByTaggerName
	);
	
	// add possible quantities for the lambda ntuples consumers
	std::string bTaggedJetCSVName = settings.GetBTaggedJetCombinedSecondaryVertexName();
	std::string bTaggedJetTCHEName = settings.GetBTaggedJetTrackCountingHighEffName();
	std::string jetPuJetIDName = settings.GetPuJetIDFullDiscrName();

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingJetCSV",[bTaggedJetCSVName](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validJets.size() >= 1 ? static_cast<KJet*>(product.m_validJets.at(0))->getTag(bTaggedJetCSVName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	} );
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingJetTCHE",[bTaggedJetTCHEName](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validJets.size() >= 1 ? static_cast<KJet*>(product.m_validJets.at(0))->getTag(bTaggedJetTCHEName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	} );
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingJetPuID",[jetPuJetIDName](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validJets.size() >= 1 ? static_cast<KJet*>(product.m_validJets.at(0))->getTag(jetPuJetIDName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	} );
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("leadingJetGenMatch", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validJets.size() >= 1 ? static_cast<KJet*>(product.m_validJets.at(0))->genMatch : false;
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingJetCSV",[bTaggedJetCSVName](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validJets.size() >= 2 ? static_cast<KJet*>(product.m_validJets.at(1))->getTag(bTaggedJetCSVName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	} );
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingJetTCHE",[bTaggedJetTCHEName](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validJets.size() >= 2 ? static_cast<KJet*>(product.m_validJets.at(1))->getTag(bTaggedJetTCHEName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	} );
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingJetPuID",[jetPuJetIDName](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validJets.size() >= 2 ? static_cast<KJet*>(product.m_validJets.at(1))->getTag(jetPuJetIDName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	} );
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("trailingJetGenMatch", [](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validJets.size() >= 2 ? static_cast<KJet*>(product.m_validJets.at(1))->genMatch : false;
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("thirdJetCSV",[bTaggedJetCSVName](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validJets.size() >= 3 ? static_cast<KJet*>(product.m_validJets.at(2))->getTag(bTaggedJetCSVName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("fourthJetCSV",[bTaggedJetCSVName](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validJets.size() >= 4 ? static_cast<KJet*>(product.m_validJets.at(3))->getTag(bTaggedJetCSVName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	});
}

// Can be overwritten for analysis-specific use cases
bool ValidTaggedJetsProducer::AdditionalCriteria(KJet* jet, KappaEvent const& event,
                                                 KappaProduct& product, KappaSettings const& settings) const
{
	assert(event.m_jetMetadata);
	
	bool validJet = ValidJetsProducerBase<KJet, KBasicJet>::AdditionalCriteria(jet, event, product, settings);
	
	// PU Jet ID
	for (std::map<size_t, std::vector<std::string> >::const_iterator puJetIdByIndex = puJetIdsByIndex.begin();
	     puJetIdByIndex != puJetIdsByIndex.end() && validJet; ++puJetIdByIndex)
	{
		if (puJetIdByIndex->first == product.m_validJets.size())
		{
			validJet = validJet && PassPuJetIds(jet, puJetIdByIndex->second, event.m_jetMetadata);
		}
	}
	
	for (std::map<std::string, std::vector<std::string> >::const_iterator puJetIdByHltName = puJetIdsByHltName.begin();
	     puJetIdByHltName != puJetIdsByHltName.end() && validJet; ++puJetIdByHltName)
	{
		if (puJetIdByHltName->first == "default")
		{
			validJet = validJet && PassPuJetIds(jet, puJetIdByHltName->second, event.m_jetMetadata);
		}
		else
		{
			LOG(FATAL) << "HLT name dependent PU Jet is not yet implemented!";
		}
	}
	
	// Jet taggers
	for (std::map<std::string, std::vector<float> >::const_iterator jetTaggerLowerCut = jetTaggerLowerCutsByTaggerName.begin();
	     jetTaggerLowerCut != jetTaggerLowerCutsByTaggerName.end() && validJet; ++jetTaggerLowerCut)
	{
		float maxLowerCut = *std::max_element(jetTaggerLowerCut->second.begin(), jetTaggerLowerCut->second.end());
		validJet = validJet && jet->getTag(jetTaggerLowerCut->first, event.m_jetMetadata) > maxLowerCut;
	}
	
	for (std::map<std::string, std::vector<float> >::const_iterator jetTaggerUpperCut = jetTaggerUpperCutsByTaggerName.begin();
	     jetTaggerUpperCut != jetTaggerUpperCutsByTaggerName.end() && validJet; ++jetTaggerUpperCut)
	{
		float minUpperCut = *std::min_element(jetTaggerUpperCut->second.begin(), jetTaggerUpperCut->second.end());
		validJet = validJet && jet->getTag(jetTaggerUpperCut->first, event.m_jetMetadata) < minUpperCut;
	}
	
	return validJet;
}

bool ValidTaggedJetsProducer::PassPuJetIds(KJet* jet, std::vector<std::string> const& puJetIds, KJetMetadata* taggerMetadata) const
{
	bool validJet = true;
	
	for (std::vector<std::string>::const_iterator puJetId = puJetIds.begin();
	     puJetId != puJetIds.end() && validJet; ++puJetId)
	{
		validJet = validJet && jet->getId(*puJetId, taggerMetadata);
	}
	
	return validJet;
}

