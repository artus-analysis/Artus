
#include "Artus/KappaAnalysis/interface/Producers/GenMuonFSRProducer.h"


std::string GenMuonFSRProducer::GetProducerId() const{
	return "GenMuonFSRProducer";
}

void GenMuonFSRProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);
	
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("leadingMuonFSR", [](event_type const& event, product_type const& product)
	{
		return product.m_sumMuonFSRPt[0];
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("trailingMuonFSR", [](event_type const& event, product_type const& product)
	{
		return product.m_sumMuonFSRPt[1];
	});
}

void GenMuonFSRProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
{
	// start with empty vector	
	product.m_sumMuonFSRPt.clear();
	
	//Loop through all valid Muons
	for (typename std::vector<KMuon*>::const_iterator validMuon = product.m_validMuons.begin();
	     validMuon != product.m_validMuons.end(); ++validMuon)
	{
		//Look for matched genMuon
		KGenParticle* currentGenParticle = SafeMap::GetWithDefault((product.m_genParticleMatchedMuons), *validMuon, (KGenParticle*)(0));
		float sumMuonFSRPt = 0;
		//for ordered readout
		//double muonPt = (*validMuon)->p4.Pt();
		if (currentGenParticle != 0)
		{
			//if 2 daughters: FSR radiation -> add pt to sumMuonFSRPt
			if (currentGenParticle->nDaughters() == 2 and abs(currentGenParticle->pdgId())==13)
			{
				//first daughter: Muon itself
				//second daughter: Photon
				if (event.m_genParticles->at(currentGenParticle->daughterIndex(1)).pdgId() == 22)
				{
					sumMuonFSRPt += event.m_genParticles->at(currentGenParticle->daughterIndex(1)).p4.Pt();
				}
				currentGenParticle = &(event.m_genParticles->at(currentGenParticle->daughterIndex(0)));
			}
		}
		product.m_sumMuonFSRPt.push_back(sumMuonFSRPt);
		//product.m_MuonPt.push_back(muonPt);
	}

	//if (product.m_MuonPt.at(0) < product.m_MuonPt.at(1))
	//	std::cout << "#ojh2o34ij523oi4j";

	
	
}


