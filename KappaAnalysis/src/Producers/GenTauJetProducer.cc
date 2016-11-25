
#include "Artus/KappaAnalysis/interface/Producers/GenTauJetProducer.h"


void GenTauJetProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);
	
	LambdaNtupleConsumer<KappaTypes>::AddVFloatQuantity("genTauJetVisPt", [](KappaEvent const & event, KappaProduct const & product)
	{
		std::vector<float> genTauJetPt;
		for (typename std::vector<KGenJet*>::const_iterator genJet = (product.m_genTauJets).begin();
		     genJet != (product.m_genTauJets).end(); ++genJet)
		{
			genTauJetPt.push_back((*genJet)->p4.Pt());
		}
		return genTauJetPt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddVFloatQuantity("genTauJetEta", [](KappaEvent const & event, KappaProduct const & product)
	{
		std::vector<float> genTauJetEta;
		for (typename std::vector<KGenJet*>::const_iterator genJet = (product.m_genTauJets).begin();
		     genJet != (product.m_genTauJets).end(); ++genJet)
		{
			genTauJetEta.push_back((*genJet)->p4.Eta());
		}
		return genTauJetEta;
	});
	LambdaNtupleConsumer<KappaTypes>::AddVIntQuantity("genTauJetDM", [](KappaEvent const & event, KappaProduct const & product)
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

void GenTauJetProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
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
