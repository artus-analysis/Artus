
#include "Artus/KappaAnalysis/interface/Producers/GenTauDecayProducer.h"

std::string GenTauDecayProducer::GetProducerId() const {
	return "GenTauDecayProducer";
}

void GenTauDecayProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);
	m_bosonPdgId = settings.GetBosonPdgId();
	m_bosonStatus = settings.GetBosonStatus();

	// add possible quantities for the lambda ntuples consumers
	
//Boson
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genBosonSize", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBoson.size() > 0 ? product.m_genBoson.size() : DefaultValues::UndefinedInt;
	});
	/*
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBosonPt", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBoson.size() > 0 ? product.m_genBoson[0].m_node->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBosonPz", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBoson.size() > 0 ? product.m_genBoson[0].m_node->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBosonEta", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBoson.size() > 0 ? product.m_genBoson[0].m_node->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBosonPhi", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBoson.size() > 0 ? product.m_genBoson[0].m_node->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBosonMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBoson.size() > 0 ? product.m_genBoson[0].m_node->p4.mass() : DefaultValues::UndefinedFloat;
	});
		LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBosonEnergy", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBoson.size() > 0 ? product.m_genBoson[0].m_node->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBosonPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBoson.size() > 0 ? product.m_genBoson[0].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBosonStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_genBoson.size() > 0 ? product.m_genBoson[0].m_node->status() : DefaultValues::UndefinedInt;
	});
	
	// Boson daughters
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBosonDaughterSize", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters.size() : DefaultValues::UndefinedInt;
	});

	// first daughter
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1DaughterPt", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_node->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1DaughterPz", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_node->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1DaughterEta", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_node->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1DaughterPhi", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_node->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1DaughterMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_node->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1DaughterCharge", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].GetCharge() : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1DaughterEnergy", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_node->p4.E() : DefaultValues::UndefinedFloat;
	});	
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1DaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1DaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_node->status() : DefaultValues::UndefinedInt;
	});

	// second daughter
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2DaughterPt", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_node->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2DaughterPz", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_node->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2DaughterEta", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_node->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2DaughterPhi", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_node->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2DaughterMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_node->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2DaughterEnergy", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_node->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2DaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2DaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_node->status() : DefaultValues::UndefinedInt;
	});

	// Boson granddaughters
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1DaughterGranddaughterSize", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_daughters.size() : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson2DaughterGranddaughterSize", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[1].m_daughters.size() : DefaultValues::UndefinedInt;
	});

	// first daughter daughters
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter1GranddaughterPt", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_daughters[0].m_node->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter1GranddaughterPz", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_daughters[0].m_node->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter1GranddaughterEta", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_daughters[0].m_node->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter1GranddaughterPhi", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_daughters[0].m_node->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter1GranddaughterMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_daughters[0].m_node->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter1GranddaughterEnergy", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_daughters[0].m_node->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter1GranddaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_daughters[0].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter1GranddaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[0].m_daughters[0].m_node->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter2GranddaughterPt", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_node->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter2GranddaughterPz", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_node->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter2GranddaughterEta", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_node->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter2GranddaughterPhi", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_node->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter2GranddaughterMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_node->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter2GranddaughterEnergy", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_node->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter2GranddaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter2GranddaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_node->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter3GranddaughterPt", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[0].m_daughters[2].m_node->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter3GranddaughterPz", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[0].m_daughters[2].m_node->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter3GranddaughterEta", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[0].m_daughters[2].m_node->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter3GranddaughterPhi", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[0].m_daughters[2].m_node->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter3GranddaughterMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[0].m_daughters[2].m_node->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter3GranddaughterEnergy", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[0].m_daughters[2].m_node->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter3GranddaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[0].m_daughters[2].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter3GranddaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[0].m_daughters[2].m_node->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter4GranddaughterPt", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[0].m_daughters[3].m_node->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter4GranddaughterPz", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[0].m_daughters[3].m_node->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter4GranddaughterEta", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[0].m_daughters[3].m_node->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter4GranddaughterPhi", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[0].m_daughters[3].m_node->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter4GranddaughterMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[0].m_daughters[3].m_node->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson1Daughter4GranddaughterEnergy", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[0].m_daughters[3].m_node->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter4GranddaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[0].m_daughters[3].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter4GranddaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[0].m_daughters[3].m_node->status() : DefaultValues::UndefinedInt;
	});

	// second daughter daughters
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter1GranddaughterPt", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[1].m_daughters[0].m_node->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter1GranddaughterPz", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[1].m_daughters[0].m_node->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter1GranddaughterEta", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[1].m_daughters[0].m_node->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter1GranddaughterPhi", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[1].m_daughters[0].m_node->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter1GranddaughterMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[1].m_daughters[0].m_node->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter1GranddaughterEnergy", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[1].m_daughters[0].m_node->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson2Daughter1GranddaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[1].m_daughters[0].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson2Daughter1GranddaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 0) ? product.m_genBoson[0].m_daughters[1].m_daughters[0].m_node->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter2GranddaughterPt", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_daughters[1].m_node->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter2GranddaughterPz", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_daughters[1].m_node->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter2GranddaughterEta", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_daughters[1].m_node->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter2GranddaughterPhi", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_daughters[1].m_node->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter2GranddaughterMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_daughters[1].m_node->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter2GranddaughterEnergy", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_daughters[1].m_node->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson2Daughter2GranddaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_daughters[1].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson2Daughter2GranddaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 1) ? product.m_genBoson[0].m_daughters[1].m_daughters[1].m_node->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter3GranddaughterPt", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[1].m_daughters[2].m_node->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter3GranddaughterPz", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[1].m_daughters[2].m_node->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter3GranddaughterEta", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[1].m_daughters[2].m_node->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter3GranddaughterPhi", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[1].m_daughters[2].m_node->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter3GranddaughterMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[1].m_daughters[2].m_node->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter3GranddaughterEnergy", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[1].m_daughters[2].m_node->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson2Daughter3GranddaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[1].m_daughters[2].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson2Daughter3GranddaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 2) ? product.m_genBoson[0].m_daughters[1].m_daughters[2].m_node->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter4GranddaughterPt", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[1].m_daughters[3].m_node->p4.Pt() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter4GranddaughterPz", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[1].m_daughters[3].m_node->p4.Pz() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter4GranddaughterEta", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[1].m_daughters[3].m_node->p4.Eta() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter4GranddaughterPhi", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[1].m_daughters[3].m_node->p4.Phi() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter4GranddaughterMass", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[1].m_daughters[3].m_node->p4.mass() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddFloatQuantity("1genBoson2Daughter4GranddaughterEnergy", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[1].m_daughters[3].m_node->p4.E() : DefaultValues::UndefinedFloat;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson2Daughter4GranddaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[1].m_daughters[3].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson2Daughter4GranddaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 3) ? product.m_genBoson[0].m_daughters[1].m_daughters[3].m_node->status() : DefaultValues::UndefinedInt;
	});

	// Boson GrandGranddaughters: the only GrandGranddaughters we need are from 2nd Granddaughters
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter2GranddaughterGrandGranddaughterSize", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters.size() >0)? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters.size() : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson2Daughter2GranddaughterGrandGranddaughterSize", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[1].m_daughters[1].m_daughters.size() >0)? product.m_genBoson[0].m_daughters[1].m_daughters[1].m_daughters.size() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter2Granddaughter1GrandGranddaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters.size() >0)? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters[0].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter2Granddaughter1GrandGranddaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters.size() >0)? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters[0].m_node->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter2Granddaughter2GrandGranddaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters.size() >1)? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters[1].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter2Granddaughter2GrandGranddaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters.size() >1)? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters[1].m_node->status() : DefaultValues::UndefinedInt;
	});

	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter2Granddaughter3GrandGranddaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters.size() >2)? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters[2].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter2Granddaughter3GrandGranddaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters.size() >2)? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters[2].m_node->status() : DefaultValues::UndefinedInt;
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter2Granddaughter4GrandGranddaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters.size() >3)? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters[3].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter2Granddaughter4GrandGranddaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters.size() >3)? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters[3].m_node->status() : DefaultValues::UndefinedInt;
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter2Granddaughter5GrandGranddaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters.size() >4)? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters[4].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter2Granddaughter5GrandGranddaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters.size() >4)? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters[4].m_node->status() : DefaultValues::UndefinedInt;
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter2Granddaughter6GrandGranddaughterPdgId", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters.size() >5)? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters[5].m_node->pdgId : DefaultValues::UndefinedInt;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("1genBoson1Daughter2Granddaughter6GrandGranddaughterStatus", [](KappaEvent const & event, KappaProduct const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].m_daughters.size() > 0) && (product.m_genBoson[0].m_daughters[0].m_daughters.size() > 1) && (product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters.size() >5)? product.m_genBoson[0].m_daughters[0].m_daughters[1].m_daughters[5].m_node->status() : DefaultValues::UndefinedInt;
	});
	*/
}


void GenTauDecayProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                  KappaSettings const& settings) const
{
	assert(event.m_genParticles);
	for (KGenParticles::iterator part = event.m_genParticles->begin();
		 part != event.m_genParticles->end(); ++part)
	{
		// If negative m_bosonStatus is set, then the status of the particle is ignored
		bool checkBosonStatus = (m_bosonStatus < 0) ? true : (part->status() == m_bosonStatus);
		unsigned int partIndex = part - event.m_genParticles->begin();
		// Filling Boson (e.g. Higgs, Z, ...), its daughter & granddaughter particles in recursive way.
		if ((std::abs(part->pdgId) == m_bosonPdgId) && checkBosonStatus)
		{
			product.m_genBoson.push_back( MotherDaughterBundle(&(*part)) );
			MotherDaughterBundle & lastBosonRef = product.m_genBoson.back();
			lastBosonRef.SetCharge();
			lastBosonRef.SetDetectable();
			if (part->daughterIndices.size() == 0) lastBosonRef.m_finalState = true;
			else BuildDecayTree(lastBosonRef, partIndex, event);
		}
	}
}
	
void GenTauDecayProducer::BuildDecayTree(MotherDaughterBundle & lastProductParentRef, unsigned int lastEventParentIndex, event_type const& event) const
{
	for (unsigned int j=0; j<(event.m_genParticles->at(lastEventParentIndex)).daughterIndices.size() && (event.m_genParticles->at(lastEventParentIndex)).daughterIndices.size() != 0; ++j)
	{
		unsigned int DaughterIndex = (event.m_genParticles->at(lastEventParentIndex)).daughterIndex(j);
		if (DaughterIndex < event.m_genParticles->size())
		{
			lastProductParentRef.m_daughters.push_back(MotherDaughterBundle( &(event.m_genParticles->at(DaughterIndex)) ));
			MotherDaughterBundle & lastDaughterRef = lastProductParentRef.m_daughters.back();
			lastDaughterRef.SetCharge();
			lastDaughterRef.SetDetectable();
			if ( (event.m_genParticles->at(DaughterIndex)).daughterIndices.size() == 0) lastDaughterRef.m_finalState = true;
			else BuildDecayTree(lastDaughterRef, DaughterIndex, event);
		}
		else
		{
			LOG(ERROR) << "Index larger than size of gen particle vector:" << DaughterIndex << ">" << event.m_genParticles->size() << ".";
		}
	}
}
