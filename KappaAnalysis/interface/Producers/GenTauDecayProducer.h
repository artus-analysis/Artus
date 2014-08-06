
#pragma once

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/Consumer/interface/LambdaNtupleConsumer.h"
#include "Artus/KappaAnalysis/interface/KappaProduct.h"


/**
   \brief GlobalProducer, for tau decays on generator level.
   
   This producers has the GenParticles as input and will extract the following information from 
   this collection :

   - tree with three generations of decay products : Boson, Bosondaughters, Bosongranddaughters  

   If need arises to store other decay trees, this code can be made more general and
   configurable.
*/

template<class TTypes>
class GenTauDecayProducer: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "gen_taudecay";
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::Init(settings);
		
		// add possible quantities for the lambda ntuples consumers
		
		//Boson
		LambdaNtupleConsumer<TTypes>::Quantities["genBosonSize"] = [](event_type const & event, product_type const & product)
		{
			return product.m_genBoson.size() > 0 ? product.m_genBoson.size() : DefaultValues::UndefinedDouble;
		};
		LambdaNtupleConsumer<TTypes>::Quantities["1genBosonPt"] = [](event_type const & event, product_type const & product)
		{
			return product.m_genBoson.size() > 0 ? product.m_genBoson[0].node->p4.Pt() : DefaultValues::UndefinedDouble;
		};
		LambdaNtupleConsumer<TTypes>::Quantities["1genBosonPz"] = [](event_type const & event, product_type const & product)
		{
			return product.m_genBoson.size() > 0 ? product.m_genBoson[0].node->p4.Pz() : DefaultValues::UndefinedDouble;
		};
		LambdaNtupleConsumer<TTypes>::Quantities["1genBosonEta"] = [](event_type const & event, product_type const & product)
		{
			return product.m_genBoson.size() > 0 ? product.m_genBoson[0].node->p4.Eta() : DefaultValues::UndefinedDouble;
		};
		LambdaNtupleConsumer<TTypes>::Quantities["1genBosonPhi"] = [](event_type const & event, product_type const & product)
		{
			return product.m_genBoson.size() > 0 ? product.m_genBoson[0].node->p4.Phi() : DefaultValues::UndefinedDouble;
		};
		LambdaNtupleConsumer<TTypes>::Quantities["1genBosonMass"] = [](event_type const & event, product_type const & product)
		{
			return product.m_genBoson.size() > 0 ? product.m_genBoson[0].node->p4.mass() : DefaultValues::UndefinedDouble;
		};
			LambdaNtupleConsumer<TTypes>::Quantities["1genBosonEnergy"] = [](event_type const & event, product_type const & product)
		{
			return product.m_genBoson.size() > 0 ? product.m_genBoson[0].node->p4.E() : DefaultValues::UndefinedDouble;
		};
		LambdaNtupleConsumer<TTypes>::Quantities["1genBosonPdgId"] = [](event_type const & event, product_type const & product)
		{
			return product.m_genBoson.size() > 0 ? product.m_genBoson[0].node->pdgId() : DefaultValues::UndefinedDouble;
		};
		LambdaNtupleConsumer<TTypes>::Quantities["1genBosonStatus"] = [](event_type const & event, product_type const & product)
		{
			return product.m_genBoson.size() > 0 ? product.m_genBoson[0].node->status() : DefaultValues::UndefinedDouble;
		};
	
	// Boson daughters
	LambdaNtupleConsumer<TTypes>::Quantities["1genBosonDaughterSize"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters.size() : DefaultValues::UndefinedDouble;
	};

	// first daughter
	LambdaNtupleConsumer<TTypes>::Quantities["TauMinusParent"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].parent->node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1DaughterPt"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].node->p4.Pt() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1DaughterPz"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].node->p4.Pz() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1DaughterEta"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].node->p4.Eta() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1DaughterPhi"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].node->p4.Phi() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1DaughterMass"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].node->p4.mass() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1DaughterCharge"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].getCharge() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1DaughterEnergy"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].node->p4.E() : DefaultValues::UndefinedDouble;
	};	
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1DaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1DaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].node->status() : DefaultValues::UndefinedDouble;
	};

	// second daughter
	LambdaNtupleConsumer<TTypes>::Quantities["TauPlusParent"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].parent->node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2DaughterPt"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].node->p4.Pt() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2DaughterPz"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].node->p4.Pz() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2DaughterEta"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].node->p4.Eta() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2DaughterPhi"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].node->p4.Phi() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2DaughterMass"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].node->p4.mass() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2DaughterEnergy"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].node->p4.E() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2DaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2DaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].node->status() : DefaultValues::UndefinedDouble;
	};

	// Boson granddaughters
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1DaughterGranddaughterSize"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].Daughters.size() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2DaughterGranddaughterSize"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[1].Daughters.size() : DefaultValues::UndefinedDouble;
	};

	// first daughter daughters
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter1GranddaughterPt"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].Daughters[0].node->p4.Pt() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter1GranddaughterPz"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].Daughters[0].node->p4.Pz() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter1GranddaughterEta"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].Daughters[0].node->p4.Eta() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter1GranddaughterPhi"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].Daughters[0].node->p4.Phi() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter1GranddaughterMass"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].Daughters[0].node->p4.mass() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter1GranddaughterEnergy"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].Daughters[0].node->p4.E() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter1GranddaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].Daughters[0].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter1GranddaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[0].Daughters[0].node->status() : DefaultValues::UndefinedDouble;
	};

	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2GranddaughterPt"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[0].Daughters[1].node->p4.Pt() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2GranddaughterPz"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[0].Daughters[1].node->p4.Pz() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2GranddaughterEta"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[0].Daughters[1].node->p4.Eta() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2GranddaughterPhi"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[0].Daughters[1].node->p4.Phi() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2GranddaughterMass"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[0].Daughters[1].node->p4.mass() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2GranddaughterEnergy"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[0].Daughters[1].node->p4.E() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2GranddaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[0].Daughters[1].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2GranddaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[0].Daughters[1].node->status() : DefaultValues::UndefinedDouble;
	};

	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter3GranddaughterPt"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[0].Daughters[2].node->p4.Pt() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter3GranddaughterPz"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[0].Daughters[2].node->p4.Pz() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter3GranddaughterEta"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[0].Daughters[2].node->p4.Eta() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter3GranddaughterPhi"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[0].Daughters[2].node->p4.Phi() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter3GranddaughterMass"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[0].Daughters[2].node->p4.mass() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter3GranddaughterEnergy"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[0].Daughters[2].node->p4.E() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter3GranddaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[0].Daughters[2].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter3GranddaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[0].Daughters[2].node->status() : DefaultValues::UndefinedDouble;
	};

	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter4GranddaughterPt"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[0].Daughters[3].node->p4.Pt() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter4GranddaughterPz"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[0].Daughters[3].node->p4.Pz() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter4GranddaughterEta"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[0].Daughters[3].node->p4.Eta() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter4GranddaughterPhi"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[0].Daughters[3].node->p4.Phi() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter4GranddaughterMass"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[0].Daughters[3].node->p4.mass() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter4GranddaughterEnergy"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[0].Daughters[3].node->p4.E() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter4GranddaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[0].Daughters[3].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter4GranddaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[0].Daughters[3].node->status() : DefaultValues::UndefinedDouble;
	};

	// second daughter daughters
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter1GranddaughterPt"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[1].Daughters[0].node->p4.Pt() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter1GranddaughterPz"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[1].Daughters[0].node->p4.Pz() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter1GranddaughterEta"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[1].Daughters[0].node->p4.Eta() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter1GranddaughterPhi"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[1].Daughters[0].node->p4.Phi() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter1GranddaughterMass"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[1].Daughters[0].node->p4.mass() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter1GranddaughterEnergy"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[1].Daughters[0].node->p4.E() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter1GranddaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[1].Daughters[0].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter1GranddaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 0) ? product.m_genBoson[0].Daughters[1].Daughters[0].node->status() : DefaultValues::UndefinedDouble;
	};

	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter2GranddaughterPt"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].Daughters[1].node->p4.Pt() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter2GranddaughterPz"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].Daughters[1].node->p4.Pz() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter2GranddaughterEta"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].Daughters[1].node->p4.Eta() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter2GranddaughterPhi"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].Daughters[1].node->p4.Phi() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter2GranddaughterMass"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].Daughters[1].node->p4.mass() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter2GranddaughterEnergy"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].Daughters[1].node->p4.E() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter2GranddaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].Daughters[1].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter2GranddaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 1) ? product.m_genBoson[0].Daughters[1].Daughters[1].node->status() : DefaultValues::UndefinedDouble;
	};

	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter3GranddaughterPt"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[1].Daughters[2].node->p4.Pt() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter3GranddaughterPz"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[1].Daughters[2].node->p4.Pz() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter3GranddaughterEta"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[1].Daughters[2].node->p4.Eta() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter3GranddaughterPhi"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[1].Daughters[2].node->p4.Phi() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter3GranddaughterMass"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[1].Daughters[2].node->p4.mass() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter3GranddaughterEnergy"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[1].Daughters[2].node->p4.E() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter3GranddaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[1].Daughters[2].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter3GranddaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 2) ? product.m_genBoson[0].Daughters[1].Daughters[2].node->status() : DefaultValues::UndefinedDouble;
	};

	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter4GranddaughterPt"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[1].Daughters[3].node->p4.Pt() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter4GranddaughterPz"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[1].Daughters[3].node->p4.Pz() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter4GranddaughterEta"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[1].Daughters[3].node->p4.Eta() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter4GranddaughterPhi"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[1].Daughters[3].node->p4.Phi() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter4GranddaughterMass"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[1].Daughters[3].node->p4.mass() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter4GranddaughterEnergy"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[1].Daughters[3].node->p4.E() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter4GranddaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[1].Daughters[3].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter4GranddaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 3) ? product.m_genBoson[0].Daughters[1].Daughters[3].node->status() : DefaultValues::UndefinedDouble;
	};

	// Boson GrandGranddaughters: the only GrandGranddaughters we need are from 2nd Granddaughters
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2GranddaughterGrandGranddaughterSize"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[0].Daughters[1].Daughters.size() >0)? product.m_genBoson[0].Daughters[0].Daughters[1].Daughters.size() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson2Daughter2GranddaughterGrandGranddaughterSize"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[1].Daughters[1].Daughters.size() >0)? product.m_genBoson[0].Daughters[1].Daughters[1].Daughters.size() : DefaultValues::UndefinedDouble;
	};

	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2Granddaughter1GrandGranddaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[0].Daughters[1].Daughters.size() >0)? product.m_genBoson[0].Daughters[0].Daughters[1].Daughters[0].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2Granddaughter1GrandGranddaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[0].Daughters[1].Daughters.size() >0)? product.m_genBoson[0].Daughters[0].Daughters[1].Daughters[0].node->status() : DefaultValues::UndefinedDouble;
	};

	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2Granddaughter2GrandGranddaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[0].Daughters[1].Daughters.size() >1)? product.m_genBoson[0].Daughters[0].Daughters[1].Daughters[1].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2Granddaughter2GrandGranddaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[0].Daughters[1].Daughters.size() >1)? product.m_genBoson[0].Daughters[0].Daughters[1].Daughters[1].node->status() : DefaultValues::UndefinedDouble;
	};

	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2Granddaughter3GrandGranddaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[0].Daughters[1].Daughters.size() >2)? product.m_genBoson[0].Daughters[0].Daughters[1].Daughters[2].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2Granddaughter3GrandGranddaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[0].Daughters[1].Daughters.size() >2)? product.m_genBoson[0].Daughters[0].Daughters[1].Daughters[2].node->status() : DefaultValues::UndefinedDouble;
	};
	
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2Granddaughter4GrandGranddaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[0].Daughters[1].Daughters.size() >3)? product.m_genBoson[0].Daughters[0].Daughters[1].Daughters[3].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2Granddaughter4GrandGranddaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[0].Daughters[1].Daughters.size() >3)? product.m_genBoson[0].Daughters[0].Daughters[1].Daughters[3].node->status() : DefaultValues::UndefinedDouble;
	};
	
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2Granddaughter5GrandGranddaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[0].Daughters[1].Daughters.size() >4)? product.m_genBoson[0].Daughters[0].Daughters[1].Daughters[4].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2Granddaughter5GrandGranddaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[0].Daughters[1].Daughters.size() >4)? product.m_genBoson[0].Daughters[0].Daughters[1].Daughters[4].node->status() : DefaultValues::UndefinedDouble;
	};
	
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2Granddaughter6GrandGranddaughterPdgId"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[0].Daughters[1].Daughters.size() >5)? product.m_genBoson[0].Daughters[0].Daughters[1].Daughters[5].node->pdgId() : DefaultValues::UndefinedDouble;
	};
	LambdaNtupleConsumer<TTypes>::Quantities["1genBoson1Daughter2Granddaughter6GrandGranddaughterStatus"] = [](event_type const & event, product_type const & product)
	{
		return (product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 0) && (product.m_genBoson[0].Daughters[0].Daughters.size() > 1) && (product.m_genBoson[0].Daughters[0].Daughters[1].Daughters.size() >5)? product.m_genBoson[0].Daughters[0].Daughters[1].Daughters[5].node->status() : DefaultValues::UndefinedDouble;
	};
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const
	{
		// Reading Boson PdgId from TauSpinnerSettings.json
		int bosonPdgId = settings.GetBosonPdgId();

		for (KGenParticles::iterator part = event.m_genParticles->begin();
			 part != event.m_genParticles->end(); ++part)
		{
			// Filling Higgs, its daughter & granddaughter particles
			if ((abs(part->pdgId()) == bosonPdgId)&&(part->status()==3))// only Boson with status 3 are considered as root of the tree.
			{
				
				product.m_genBoson.push_back( MotherDaughterBundle(&(*part)) );
				MotherDaughterBundle & lastBosonRef = product.m_genBoson.back();
				lastBosonRef.parent = &lastBosonRef;
				//std::cout << &(lastBosonRef) << std::endl;
				lastBosonRef.setCharge();
				lastBosonRef.setDetectable();
				if (part->daughterIndices.size() == 0) lastBosonRef.finalState = true;
				for (unsigned int i=0; i<part->daughterIndices.size() && part->daughterIndices.size() != 0; ++i) 
				{
					// Higgs with Status 2 is also considered as Higgs status 3 daughter, what leads to the condition, 
					// that we search only for Higgs daughters with PdgId != 25.
					unsigned int indDaughter = part->daughterIndex(i);
					if ( indDaughter < event.m_genParticles->size() )
					{
						// Taus with status 2 are the only daughters of Taus with status 3. We are not interested in status 2 Taus and thats the reason, why we should  
						// skip them and consider the formal granddaughters of status 3 Taus as real daughters of status 3 Taus. This means, we must skip one generation,
						// what's done in the following lines.
						lastBosonRef.Daughters.push_back(MotherDaughterBundle( &(event.m_genParticles->at(indDaughter)) ));
						MotherDaughterBundle & lastBosonDaughterRef = lastBosonRef.Daughters.back();
						//std::cout << &(lastBosonDaughterRef) << std::endl;
						lastBosonDaughterRef.parent = &lastBosonRef;
						lastBosonDaughterRef.setCharge();
						lastBosonDaughterRef.setDetectable();
						if ( (event.m_genParticles->at(indDaughter)).daughterIndices.size() != 0)
						{
							unsigned int indDaughterStat2 = (event.m_genParticles->at(indDaughter)).daughterIndex(0);
							BuildDecayTree(lastBosonDaughterRef, indDaughterStat2, event);
						}
						else lastBosonDaughterRef.finalState = true;
					}
					else if (!(indDaughter < event.m_genParticles->size()))
					{
					LOG(ERROR) << "Index larger than size of gen particle vector:" << indDaughter << ">" << event.m_genParticles->size() << ".";
					}
				}
			}
		}
	//std::cout << "Tau1: " << &(product.m_genBoson[0].Daughters[0]) << std::endl;
	//std::cout << "parent ueber Daughter: " << product.m_genBoson[0].Daughters[0].Daughters[0].parent << std::endl;
	//std::cout << std::endl;
	}
	

private:
	void BuildDecayTree(MotherDaughterBundle & lastProductParentRef, unsigned int lastEventParentIndex, event_type const& event) const
	{
		for (unsigned int j=0; j<(event.m_genParticles->at(lastEventParentIndex)).daughterIndices.size() && (event.m_genParticles->at(lastEventParentIndex)).daughterIndices.size() != 0; ++j)
		{
			unsigned int DaughterIndex = (event.m_genParticles->at(lastEventParentIndex)).daughterIndex(j);
			if (DaughterIndex < event.m_genParticles->size())
			{
				lastProductParentRef.Daughters.push_back(MotherDaughterBundle( &(event.m_genParticles->at(DaughterIndex)) ));
				MotherDaughterBundle & lastDaughterRef = lastProductParentRef.Daughters.back();
				lastDaughterRef.parent = &lastProductParentRef;
				//std::cout << "parent selbst im product: " << &lastProductParentRef << std::endl;
				//std::cout << "parent über Daughter im product: " << lastDaughterRef.parent << std::endl;
				lastDaughterRef.setCharge();
				lastDaughterRef.setDetectable();
				if ( (event.m_genParticles->at(DaughterIndex)).daughterIndices.size() == 0) lastDaughterRef.finalState = true;
				else BuildDecayTree(lastDaughterRef, DaughterIndex, event);
			}
			else
			{
				LOG(ERROR) << "Index larger than size of gen particle vector:" << DaughterIndex << ">" << event.m_genParticles->size() << ".";
			}
		}
	}

};

