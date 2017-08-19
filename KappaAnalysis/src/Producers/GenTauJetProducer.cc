
#include "Artus/KappaAnalysis/interface/Producers/GenTauJetProducer.h"

std::string GenTauJetProducer::GetProducerId() const{
	return "GenTauJetProducer";
}

void GenTauJetProducer::Init(setting_type const& settings, metadata_type& metadata)
{
	KappaProducerBase::Init(settings, metadata);
	
	LambdaNtupleConsumer<KappaTypes>::AddVFloatQuantity("genTauJetVisPt", [](event_type const & event, product_type const & product)
	{
		std::vector<float> genTauJetPt;
		for (typename std::vector<KGenJet*>::const_iterator genJet = (product.m_genTauJets).begin();
		     genJet != (product.m_genTauJets).end(); ++genJet)
		{
			genTauJetPt.push_back((*genJet)->p4.Pt());
		}
		return genTauJetPt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddVFloatQuantity("genTauJetEta", [](event_type const & event, product_type const & product)
	{
		std::vector<float> genTauJetEta;
		for (typename std::vector<KGenJet*>::const_iterator genJet = (product.m_genTauJets).begin();
		     genJet != (product.m_genTauJets).end(); ++genJet)
		{
			genTauJetEta.push_back((*genJet)->p4.Eta());
		}
		return genTauJetEta;
	});
	LambdaNtupleConsumer<KappaTypes>::AddVIntQuantity("genTauJetDM", [](event_type const & event, product_type const & product)
	{
		std::vector<int> genTauJetDM;
		for (typename std::vector<KGenJet*>::const_iterator genJet = (product.m_genTauJets).begin();
		     genJet != (product.m_genTauJets).end(); ++genJet)
		{
			genTauJetDM.push_back((*genJet)->genTauDecayMode);
		}
		return genTauJetDM;
	});
}

void GenTauJetProducer::Produce(event_type const& event, product_type& product,
                                setting_type const& settings, metadata_type const& metadata) const
{
	assert(event.m_genTauJets);
	
	for (KGenJets::iterator part = event.m_genTauJets->begin(); part != event.m_genTauJets->end(); ++part)
	{
		int decayMode = part->genTauDecayMode;
		
		// select only 1-prong and 3-prong decay modes
		if (((decayMode >= 0) && (decayMode <= 4)) || (decayMode == 7) || (decayMode == 8))
		{
			product.m_genTauJets.push_back(&(*part));
		}
	}
}
