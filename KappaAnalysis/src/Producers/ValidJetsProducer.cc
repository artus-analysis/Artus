
#include "Artus/KappaAnalysis/interface/Producers/ValidJetsProducer.h"


ValidJetsProducer::ValidJetsProducer() : ValidJetsProducerBase<KDataPFJet, KDataPFJet>(&KappaEvent::m_jets,
                                                                            &KappaProduct::m_correctedJets,
                                                                            &KappaProduct::m_validJets)
{
};

std::string ValidJetsProducer::GetProducerId() const {
	return "ValidJetsProducer";
}

ValidTaggedJetsProducer::ValidTaggedJetsProducer() : ValidJetsProducerBase<KDataPFTaggedJet, KDataPFJet>(&KappaEvent::m_tjets,
                                                                                        &KappaProduct::m_correctedTaggedJets,
                                                                                        &KappaProduct::m_validJets)
{
};

std::string ValidTaggedJetsProducer::GetProducerId() const {
	return "ValidTaggedJetsProducer";
}

void ValidTaggedJetsProducer::Init(KappaSettings const& settings)
{
	ValidJetsProducerBase<KDataPFTaggedJet, KDataPFJet>::Init(settings);
	
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
	LambdaNtupleConsumer<KappaTypes>::AddQuantity("leadingJetCSV",[](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validJets.size() >= 1 ? static_cast<KDataPFTaggedJet*>(product.m_validJets.at(0))->getTagger("CombinedSecondaryVertexBJetTags", event.m_taggerMetadata) : DefaultValues::UndefinedDouble;
	} );
	LambdaNtupleConsumer<KappaTypes>::AddQuantity("leadingJetTCHE",[](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validJets.size() >= 1 ? static_cast<KDataPFTaggedJet*>(product.m_validJets.at(0))->getTagger("TrackCountingHighEffBJetTags", event.m_taggerMetadata) : DefaultValues::UndefinedDouble;
	} );
	
	LambdaNtupleConsumer<KappaTypes>::AddQuantity("trailingJetCSV",[](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validJets.size() >= 2 ? static_cast<KDataPFTaggedJet*>(product.m_validJets.at(1))->getTagger("CombinedSecondaryVertexBJetTags", event.m_taggerMetadata) : DefaultValues::UndefinedDouble;
	} );
	LambdaNtupleConsumer<KappaTypes>::AddQuantity("trailingJetTCHE",[](KappaEvent const& event, KappaProduct const& product) {
		return product.m_validJets.size() >= 2 ? static_cast<KDataPFTaggedJet*>(product.m_validJets.at(1))->getTagger("TrackCountingHighEffBJetTags", event.m_taggerMetadata) : DefaultValues::UndefinedDouble;
	} );
}

// Can be overwritten for analysis-specific use cases
bool ValidTaggedJetsProducer::AdditionalCriteria(KDataPFTaggedJet* jet, KappaEvent const& event,
                                KappaProduct& product, KappaSettings const& settings) const
{
	bool validJet = ValidJetsProducerBase<KDataPFTaggedJet, KDataPFJet>::AdditionalCriteria(jet, event, product, settings);
	
	// PU Jet ID
	for (std::map<size_t, std::vector<std::string> >::const_iterator puJetIdByIndex = puJetIdsByIndex.begin();
	     puJetIdByIndex != puJetIdsByIndex.end() && validJet; ++puJetIdByIndex)
	{
		if (puJetIdByIndex->first == product.m_validJets.size())
		{
			validJet = validJet && PassPuJetIds(jet, puJetIdByIndex->second, event.m_taggerMetadata);
		}
	}
	
	for (std::map<std::string, std::vector<std::string> >::const_iterator puJetIdByHltName = puJetIdsByHltName.begin();
	     puJetIdByHltName != puJetIdsByHltName.end() && validJet; ++puJetIdByHltName)
	{
		if (puJetIdByHltName->first == "default")
		{
			validJet = validJet && PassPuJetIds(jet, puJetIdByHltName->second, event.m_taggerMetadata);
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
		validJet = validJet && jet->getTagger(jetTaggerLowerCut->first, event.m_taggerMetadata) > maxLowerCut;
	}
	
	for (std::map<std::string, std::vector<float> >::const_iterator jetTaggerUpperCut = jetTaggerUpperCutsByTaggerName.begin();
	     jetTaggerUpperCut != jetTaggerUpperCutsByTaggerName.end() && validJet; ++jetTaggerUpperCut)
	{
		float minUpperCut = *std::min_element(jetTaggerUpperCut->second.begin(), jetTaggerUpperCut->second.end());
		validJet = validJet && jet->getTagger(jetTaggerUpperCut->first, event.m_taggerMetadata) < minUpperCut;
	}
	
	return validJet;
}

bool ValidTaggedJetsProducer::PassPuJetIds(KDataPFTaggedJet* jet, std::vector<std::string> const& puJetIds, KTaggerMetadata* taggerMetadata) const
{
	bool validJet = true;
	
	for (std::vector<std::string>::const_iterator puJetId = puJetIds.begin();
	     puJetId != puJetIds.end() && validJet; ++puJetId)
	{
		validJet = validJet && jet->getpuJetID(*puJetId, taggerMetadata);
	}
	
	return validJet;
}

