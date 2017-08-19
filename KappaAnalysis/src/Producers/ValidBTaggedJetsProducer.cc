
#include "Artus/KappaAnalysis/interface/Producers/ValidBTaggedJetsProducer.h"
#include "Artus/Utility/interface/SafeMap.h"


std::string ValidBTaggedJetsProducer::GetProducerId() const {
	return "ValidBTaggedJetsProducer";
}

void ValidBTaggedJetsProducer::Init(setting_type const& settings, metadata_type& metadata)
{
	KappaProducerBase::Init(settings, metadata);
	std::map<std::string, std::vector<float> > bTagWorkingPointsTmp = Utility::ParseMapTypes<std::string, float>(
			Utility::ParseVectorToMap(settings.GetBTaggerWorkingPoints())
	);

	BTagSF bTagSFBase(settings.GetBTagScaleFactorFile(), settings.GetBTagEfficiencyFile());
	for (std::pair<std::string, std::vector<float> > bTagWorkingPoint : bTagWorkingPointsTmp)
	{
		m_bTagWorkingPoints[bTagWorkingPoint.first] = bTagWorkingPoint.second.at(0);
		if (settings.GetApplyBTagSF() && !settings.GetInputIsData())
		{
			m_bTagSFMethod = KappaEnumTypes::ToBTagScaleFactorMethod(boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(settings.GetBTagSFMethod())));
			m_bTagSfMap[bTagWorkingPoint.first] = bTagSFBase;
			m_bTagSfMap[bTagWorkingPoint.first].initBtagwp(bTagWorkingPoint.first);
		}
		// define lambda expression for nbtag per working point
		std::string btagQuantity = std::string("n")+bTagWorkingPoint.first+std::string("btag");
		LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(btagQuantity, [bTagWorkingPoint](event_type const& event, product_type const& product) {
			auto it = product.m_bTaggedJetsByWp.find(bTagWorkingPoint.first);
			return it != product.m_bTaggedJetsByWp.end() ? product.m_bTaggedJetsByWp.at(bTagWorkingPoint.first).size() : 0;
		});
	}
	
	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJets", [](event_type const& event, product_type const& product) {
		return product.m_bTaggedJets.size();
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJets20", [this](event_type const& event, product_type const& product) {
		return product_type::GetNJetsAbovePtThreshold(product.m_bTaggedJets, 20.0);
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("nBJets30", [this](event_type const& event, product_type const& product) {
		return product_type::GetNJetsAbovePtThreshold(product.m_bTaggedJets, 30.0);
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJetPt", [](event_type const& event, product_type const& product) {
		return product.m_bTaggedJets.size() >= 1 ? product.m_bTaggedJets.at(0)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJetEta", [](event_type const& event, product_type const& product) {
		return product.m_bTaggedJets.size() >= 1 ? product.m_bTaggedJets.at(0)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJetPhi", [](event_type const& event, product_type const& product) {
		return product.m_bTaggedJets.size() >= 1 ? product.m_bTaggedJets.at(0)->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJet2Pt", [](event_type const& event, product_type const& product) {
		return product.m_bTaggedJets.size() >= 2 ? product.m_bTaggedJets.at(1)->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJet2Eta", [](event_type const& event, product_type const& product) {
		return product.m_bTaggedJets.size() >= 2 ? product.m_bTaggedJets.at(1)->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("bJet2Phi", [](event_type const& event, product_type const& product) {
		return product.m_bTaggedJets.size() >= 2 ? product.m_bTaggedJets.at(1)->p4.Phi() : DefaultValues::UndefinedFloat;
	});

	std::string bTaggedJetCSVName = settings.GetBTaggedJetCombinedSecondaryVertexName();
	std::string jetPuJetIDName = settings.GetPuJetIDFullDiscrName();

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingBJetCSV",[bTaggedJetCSVName](event_type const& event, product_type const& product) {
		return product.m_bTaggedJets.size() >= 1 ? static_cast<KJet*>(product.m_bTaggedJets.at(0))->getTag(bTaggedJetCSVName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingBJetPuID",[jetPuJetIDName](event_type const& event, product_type const& product) {
		return product.m_bTaggedJets.size() >= 1 ? static_cast<KJet*>(product.m_bTaggedJets.at(0))->getTag(jetPuJetIDName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingBJetCSV",[bTaggedJetCSVName](event_type const& event, product_type const& product) {
		return product.m_bTaggedJets.size() >= 2 ? static_cast<KJet*>(product.m_bTaggedJets.at(1))->getTag(bTaggedJetCSVName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingBJetPuID",[jetPuJetIDName](event_type const& event, product_type const& product) {
		return product.m_bTaggedJets.size() >= 2 ? static_cast<KJet*>(product.m_bTaggedJets.at(1))->getTag(jetPuJetIDName, event.m_jetMetadata) : DefaultValues::UndefinedFloat;
	});
}

void ValidBTaggedJetsProducer::Produce(event_type const& event, product_type& product,
                                       setting_type const& settings, metadata_type const& metadata) const
{
	assert(event.m_jetMetadata);
	assert(settings.GetBTagWPs().size() > 0);

	for (std::vector<std::string>::const_iterator workingPoint = settings.GetBTagWPs().begin();
	     workingPoint != settings.GetBTagWPs().end(); ++workingPoint)
	{
		for (std::vector<KBasicJet*>::iterator jet = product.m_validJets.begin();
			jet != product.m_validJets.end(); ++jet)
		{
			bool validBJet = true;
			KJet* tjet = static_cast<KJet*>(*jet);

			float combinedSecondaryVertex = tjet->getTag(settings.GetBTaggedJetCombinedSecondaryVertexName(), event.m_jetMetadata);
			float bTagWorkingPoint = SafeMap::Get(m_bTagWorkingPoints, *workingPoint);

			if (combinedSecondaryVertex < bTagWorkingPoint ||
				std::abs(tjet->p4.eta()) > settings.GetBTaggedJetAbsEtaCut()) {
				validBJet = false;
			}

			validBJet = validBJet && AdditionalCriteria(tjet, event, product, settings, metadata);
			
			//entry point for Scale Factor (SF) of btagged jets
			if (settings.GetApplyBTagSF() && !settings.GetInputIsData())
			{
				//https://twiki.cern.ch/twiki/bin/view/CMS/BTagSFMethods#2a_Jet_by_jet_updating_of_the_b
				if (m_bTagSFMethod == KappaEnumTypes::BTagScaleFactorMethod::PROMOTIONDEMOTION) {
				
					int jetflavor = tjet->flavour;
					unsigned int btagSys = BTagSF::kNo;
					unsigned int bmistagSys = BTagSF::kNo;
					
					if (settings.GetBTagShift()<0)
						btagSys = BTagSF::kDown;
					if (settings.GetBTagShift()>0)
						btagSys = BTagSF::kUp;
					if (settings.GetBMistagShift()<0)
						bmistagSys = BTagSF::kDown;
					if (settings.GetBMistagShift()>0)
						bmistagSys = BTagSF::kUp;

					LOG_N_TIMES(1, DEBUG) << "Btagging shifts tag/mistag : " << settings.GetBTagShift() << " " << settings.GetBMistagShift(); 
					
					bool taggedBefore = validBJet;
					validBJet = m_bTagSfMap.at(*workingPoint).isbtagged(
							tjet->p4.pt(),
							tjet->p4.eta(),
							combinedSecondaryVertex,
							jetflavor,
							btagSys,
							bmistagSys,
							settings.GetYear(),
							bTagWorkingPoint
					);
					
					if (taggedBefore != validBJet)
						LOG_N_TIMES(20, DEBUG) << "Promoted/demoted : " << validBJet;
				}
				
				else if (m_bTagSFMethod == KappaEnumTypes::BTagScaleFactorMethod::OTHER) {
					//todo
				}
			}

			if (validBJet)
				product.m_bTaggedJetsByWp[*workingPoint].push_back(tjet);
			else
				product.m_nonBTaggedJetsByWp[*workingPoint].push_back(tjet);
		}
	}
	product.m_bTaggedJets = product.m_bTaggedJetsByWp[settings.GetBTagWPs().at(0)];
	product.m_nonBTaggedJets = product.m_nonBTaggedJetsByWp[settings.GetBTagWPs().at(0)];
}

bool ValidBTaggedJetsProducer::AdditionalCriteria(KJet* jet, event_type const& event, product_type& product,
                                                  setting_type const& settings, metadata_type const& metadata) const
{
	bool validBJet = true;
	return validBJet;
}

