
#include "Artus/KappaAnalysisExample/interface/DiMuonSystemProducer.h"

#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"


std::string DiMuonSystemProducer::GetProducerId() const
{
	return "DiMuonSystemProducer";
}

void DiMuonSystemProducer::Init(setting_type const& settings)
{
	ProducerBase<KappaExampleTypes>::Init(settings);
	
	// add possible quantities for the lambda ntuples consumers
	LambdaNtupleConsumer<KappaExampleTypes>::AddFloatQuantity("diMuonPt", [](event_type const& event, product_type const& product) {
		return product.m_diMuonSystem.Pt();
	});
	LambdaNtupleConsumer<KappaExampleTypes>::AddFloatQuantity("diMuonEta", [](event_type const& event, product_type const& product) {
		return product.m_diMuonSystem.Eta();
	});
	LambdaNtupleConsumer<KappaExampleTypes>::AddFloatQuantity("diMuonPhi", [](event_type const& event, product_type const& product) {
		return product.m_diMuonSystem.Phi();
	});
	LambdaNtupleConsumer<KappaExampleTypes>::AddFloatQuantity("diMuonMass", [](event_type const& event, product_type const& product) {
		return product.m_diMuonSystem.mass();
	});
}

void DiMuonSystemProducer::Produce(event_type const& event, product_type& product,
                                   setting_type const& settings) const
{
	// make sure that there are at least two muons reconstructed
	// this should be ensured by a muon counting filter before running this producer
	assert(product.m_validMuons.size() >= 2);
	
	// determine the di-muon system and write it to the product
	product.m_diMuonSystem = (product.m_validMuons[0]->p4 + product.m_validMuons[1]->p4);
}

