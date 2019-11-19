
#include "Artus/KappaAnalysis/interface/Producers/GenTauDecayProducer.h"
#include "Artus/Utility/interface/Utility.h"


std::string GenTauDecayProducer::GetProducerId() const {
	return "GenTauDecayProducer";
}

void GenTauDecayProducer::Init(setting_type const& settings, metadata_type& metadata)
{
	KappaProducerBase::Init(settings, metadata);

	// add possible quantities for the lambda ntuples consumers

	///*
	// Boson daughters
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBosonDaughterSize", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters.size() : DefaultValues::UndefinedInt;
	});

	// first daughter
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1DaughterPt", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_genParticle->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1DaughterPz", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_genParticle->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1DaughterEta", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_genParticle->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1DaughterPhi", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_genParticle->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1DaughterMass", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_genParticle->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1DaughterCharge", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].GetCharge() : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1DaughterEnergy", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_genParticle->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1DaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1DaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_genParticle->status() : DefaultValues::UndefinedInt;
	});

	// second daughter
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2DaughterPt", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_genParticle->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2DaughterPz", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_genParticle->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2DaughterEta", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_genParticle->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2DaughterPhi", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_genParticle->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2DaughterMass", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_genParticle->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2DaughterEnergy", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_genParticle->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2DaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2DaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_genParticle->status() : DefaultValues::UndefinedInt;
	});

	// Boson granddaughters
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1DaughterGranddaughterSize", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_daughters.size() : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson2DaughterGranddaughterSize", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[1].m_daughters.size() : DefaultValues::UndefinedInt;
	});

	// first daughter daughters
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter1GranddaughterPt", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_daughters[0].m_genParticle->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter1GranddaughterPz", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_daughters[0].m_genParticle->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter1GranddaughterEta", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_daughters[0].m_genParticle->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter1GranddaughterPhi", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_daughters[0].m_genParticle->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter1GranddaughterMass", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_daughters[0].m_genParticle->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter1GranddaughterEnergy", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_daughters[0].m_genParticle->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter1GranddaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_daughters[0].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter1GranddaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[0].m_daughters[0].m_genParticle->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter2GranddaughterPt", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_genParticle->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter2GranddaughterPz", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_genParticle->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter2GranddaughterEta", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_genParticle->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter2GranddaughterPhi", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_genParticle->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter2GranddaughterMass", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_genParticle->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter2GranddaughterEnergy", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_genParticle->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter2GranddaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter2GranddaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_genParticle->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter3GranddaughterPt", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[0].m_daughters[2].m_genParticle->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter3GranddaughterPz", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[0].m_daughters[2].m_genParticle->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter3GranddaughterEta", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[0].m_daughters[2].m_genParticle->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter3GranddaughterPhi", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[0].m_daughters[2].m_genParticle->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter3GranddaughterMass", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[0].m_daughters[2].m_genParticle->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter3GranddaughterEnergy", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[0].m_daughters[2].m_genParticle->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter3GranddaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[0].m_daughters[2].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter3GranddaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[0].m_daughters[2].m_genParticle->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter4GranddaughterPt", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[0].m_daughters[3].m_genParticle->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter4GranddaughterPz", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[0].m_daughters[3].m_genParticle->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter4GranddaughterEta", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[0].m_daughters[3].m_genParticle->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter4GranddaughterPhi", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[0].m_daughters[3].m_genParticle->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter4GranddaughterMass", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[0].m_daughters[3].m_genParticle->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson1Daughter4GranddaughterEnergy", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[0].m_daughters[3].m_genParticle->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter4GranddaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[0].m_daughters[3].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter4GranddaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[0].m_daughters[3].m_genParticle->status() : DefaultValues::UndefinedInt;
	});

	// second daughter daughters
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter1GranddaughterPt", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[1].m_daughters[0].m_genParticle->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter1GranddaughterPz", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[1].m_daughters[0].m_genParticle->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter1GranddaughterEta", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[1].m_daughters[0].m_genParticle->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter1GranddaughterPhi", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[1].m_daughters[0].m_genParticle->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter1GranddaughterMass", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[1].m_daughters[0].m_genParticle->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter1GranddaughterEnergy", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[1].m_daughters[0].m_genParticle->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson2Daughter1GranddaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[1].m_daughters[0].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson2Daughter1GranddaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 0) ? product.m_genBosonTree.m_daughters[1].m_daughters[0].m_genParticle->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter2GranddaughterPt", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_daughters[1].m_genParticle->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter2GranddaughterPz", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_daughters[1].m_genParticle->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter2GranddaughterEta", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_daughters[1].m_genParticle->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter2GranddaughterPhi", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_daughters[1].m_genParticle->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter2GranddaughterMass", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_daughters[1].m_genParticle->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter2GranddaughterEnergy", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_daughters[1].m_genParticle->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson2Daughter2GranddaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_daughters[1].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson2Daughter2GranddaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 1) ? product.m_genBosonTree.m_daughters[1].m_daughters[1].m_genParticle->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter3GranddaughterPt", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[1].m_daughters[2].m_genParticle->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter3GranddaughterPz", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[1].m_daughters[2].m_genParticle->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter3GranddaughterEta", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[1].m_daughters[2].m_genParticle->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter3GranddaughterPhi", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[1].m_daughters[2].m_genParticle->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter3GranddaughterMass", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[1].m_daughters[2].m_genParticle->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter3GranddaughterEnergy", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[1].m_daughters[2].m_genParticle->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson2Daughter3GranddaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[1].m_daughters[2].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson2Daughter3GranddaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 2) ? product.m_genBosonTree.m_daughters[1].m_daughters[2].m_genParticle->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter4GranddaughterPt", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[1].m_daughters[3].m_genParticle->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter4GranddaughterPz", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[1].m_daughters[3].m_genParticle->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter4GranddaughterEta", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[1].m_daughters[3].m_genParticle->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter4GranddaughterPhi", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[1].m_daughters[3].m_genParticle->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter4GranddaughterMass", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[1].m_daughters[3].m_genParticle->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity(metadata, "1genBoson2Daughter4GranddaughterEnergy", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[1].m_daughters[3].m_genParticle->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson2Daughter4GranddaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[1].m_daughters[3].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson2Daughter4GranddaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 3) ? product.m_genBosonTree.m_daughters[1].m_daughters[3].m_genParticle->status() : DefaultValues::UndefinedInt;
	});

	// Boson GrandGranddaughters: the only GrandGranddaughters we need are from 2nd Granddaughters
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter2GranddaughterGrandGranddaughterSize", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters.size() >0)? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters.size() : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson2Daughter2GranddaughterGrandGranddaughterSize", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[1].m_daughters[1].m_daughters.size() >0)? product.m_genBosonTree.m_daughters[1].m_daughters[1].m_daughters.size() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter2Granddaughter1GrandGranddaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters.size() >0)? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters[0].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter2Granddaughter1GrandGranddaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters.size() >0)? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters[0].m_genParticle->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter2Granddaughter2GrandGranddaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters.size() >1)? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters[1].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter2Granddaughter2GrandGranddaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters.size() >1)? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters[1].m_genParticle->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter2Granddaughter3GrandGranddaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters.size() >2)? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters[2].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter2Granddaughter3GrandGranddaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters.size() >2)? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters[2].m_genParticle->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter2Granddaughter4GrandGranddaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters.size() >3)? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters[3].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter2Granddaughter4GrandGranddaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters.size() >3)? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters[3].m_genParticle->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter2Granddaughter5GrandGranddaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters.size() >4)? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters[4].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter2Granddaughter5GrandGranddaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters.size() >4)? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters[4].m_genParticle->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter2Granddaughter6GrandGranddaughterPdgId", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters.size() >5)? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters[5].m_genParticle->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity(metadata, "1genBoson1Daughter2Granddaughter6GrandGranddaughterStatus", [](event_type const& event, product_type const& product, setting_type const& settings, metadata_type const& metadata)
	{
		return (product.m_genBosonTree.m_daughters.size() > 0) && (product.m_genBosonTree.m_daughters[0].m_daughters.size() > 1) && (product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters.size() >5)? product.m_genBosonTree.m_daughters[0].m_daughters[1].m_daughters[5].m_genParticle->status() : DefaultValues::UndefinedInt;
	});
	//*/
}


void GenTauDecayProducer::Produce(event_type const& event, product_type& product,
                                  setting_type const& settings, metadata_type const& metadata) const
{
	assert(event.m_genParticles);

	// A generator level boson or its leptonic decay products must exist
	// This is searched for by a GenBosonProducer
	if (product.m_genBosonParticle != nullptr)
	{
		product.m_genBosonTree = GenParticleDecayTree(product.m_genBosonParticle);
		product.m_genTauDecayTrees[product.m_genBosonParticle] = &product.m_genBosonTree;

		if (product.m_genBosonParticle->daughterIndices.empty())
		{
			product.m_genBosonTree.m_finalState = true;
		}
		else
		{
			BuildDecayTree(product.m_genBosonTree, product.m_genBosonParticle, event);
		}
	}
	else if (product.m_genBosonLVFound && (product.m_genLeptonsFromBosonDecay.size() >= 2))
	{
		product.m_genBosonTree = GenParticleDecayTree(nullptr);
		for (std::vector<KGenParticle*>::const_iterator genLepton = product.m_genLeptonsFromBosonDecay.begin();
		     genLepton != product.m_genLeptonsFromBosonDecay.end(); ++genLepton)
		{
			product.m_genBosonTree.m_daughters.push_back(GenParticleDecayTree(*genLepton));
			if ((*genLepton)->daughterIndices.empty())
			{
				product.m_genBosonTree.m_daughters.back().m_finalState = true;
			}
			else
			{
				BuildDecayTree(product.m_genBosonTree.m_daughters.back(), *genLepton, event);
			}
		}
	}

	for (std::vector<GenParticleDecayTree>::iterator bosonDecayProduct = product.m_genBosonTree.m_daughters.begin();
	     bosonDecayProduct != product.m_genBosonTree.m_daughters.end(); ++bosonDecayProduct)
	{
		product.m_genTauDecayTrees[bosonDecayProduct->m_genParticle] = &(*bosonDecayProduct);
	}
}

void GenTauDecayProducer::BuildDecayTree(GenParticleDecayTree& currentDecayTree, KGenParticle* currentGenParticle, event_type const& event) const
{
	for (std::vector<unsigned int>::iterator daughterIndex = currentGenParticle->daughterIndices.begin();
	     daughterIndex != currentGenParticle->daughterIndices.end(); ++daughterIndex)
	{
		KGenParticle* daughterGenParticle = &(event.m_genParticles->at(*daughterIndex));
		currentDecayTree.m_daughters.push_back(GenParticleDecayTree(daughterGenParticle));
		GenParticleDecayTree & daughterDecayTree = currentDecayTree.m_daughters.back();
		daughterDecayTree.SetCharge();
		daughterDecayTree.SetDetectable();
		if (daughterGenParticle->daughterIndices.size() == 0)
		{
			daughterDecayTree.m_finalState = true;
		}
		else
		{
			BuildDecayTree(daughterDecayTree, daughterGenParticle, event);
		}
	}
}
