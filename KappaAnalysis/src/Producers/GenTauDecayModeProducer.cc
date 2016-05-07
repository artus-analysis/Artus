
#include "Artus/KappaAnalysis/interface/Producers/GenTauDecayModeProducer.h"
#include "Artus/Utility/interface/Utility.h"
#include "Artus/Utility/interface/DefaultValues.h"

std::string GenTauDecayModeProducer::GetProducerId() const {
	return "GenTauDecayModeProducer";
}

void GenTauDecayModeProducer::Init(KappaSettings const& settings)
{
	KappaProducerBase::Init(settings);

	// add possible quantities for the lambda ntuples consumers

	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genTau1DecayMode", [](KappaEvent const & event, KappaProduct const & product)
	{
		return Utility::ToUnderlyingValue(product.m_tau1DecayMode);
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genTau2DecayMode", [](KappaEvent const & event, KappaProduct const & product)
	{
		return Utility::ToUnderlyingValue(product.m_tau2DecayMode);
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genTau1ProngSize", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_tau1ProngSize;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genTau2ProngSize", [](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_tau2ProngSize;
	});
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity("genTauTauDecayMode", [](KappaEvent const & event, KappaProduct const & product)
	{
		return Utility::ToUnderlyingValue(product.m_genTauTauDecayMode);
	});
	
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZtt", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::TT);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZmt", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::MT);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZet", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::ET);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZee", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::EE);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZmm", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::MM);
	});
	LambdaNtupleConsumer<KappaTypes>::AddBoolQuantity("isZem", [](KappaEvent const& event, KappaProduct const& product)
	{
		return (product.m_genTauTauDecayMode == KappaEnumTypes::TauTauDecayMode::EM);
	});
}

void GenTauDecayModeProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                      KappaSettings const& settings) const
{
	assert(event.m_genTaus);
	
	// A generator level boson and its decay products must exist
	// The boson is searched for by a GenBosonProducer
	// and the decay tree is built by the GenTauDecayProducer
	assert(product.m_genBosonTree.m_daughters.size() > 1);

	int tau1ProngSize = -1;
	int tau2ProngSize = -1;

	GenParticleDecayTree::DecayMode tau1DecayMode = GenParticleDecayTree::DecayMode::NONE;
	GenParticleDecayTree::DecayMode tau2DecayMode = GenParticleDecayTree::DecayMode::NONE;

	GenParticleDecayTree* selectedTau1 = &(product.m_genBosonTree.m_daughters[0]);
	GenParticleDecayTree* selectedTau2 = &(product.m_genBosonTree.m_daughters[1]);

	selectedTau1->DetermineDecayMode(selectedTau1);
	selectedTau2->DetermineDecayMode(selectedTau2);

	tau1DecayMode = selectedTau1->m_decayMode;
	tau2DecayMode = selectedTau2->m_decayMode;

	selectedTau1->CreateFinalStateProngs(selectedTau1);
	selectedTau2->CreateFinalStateProngs(selectedTau2);

	if ((selectedTau1->m_finalStateOneProngs.size()) > 0)
	{
		tau1ProngSize = 1;
	}
	else if ((selectedTau1->m_finalStateThreeProngs.size()) > 0)
	{
		tau1ProngSize = 3;
	}
	else if ((selectedTau1->m_finalStateFiveProngs.size()) > 0)
	{
		tau1ProngSize = 5;
	}

	if (selectedTau2->m_finalStateOneProngs.size() > 0)
	{
		tau2ProngSize = 1;
	}
	else if (selectedTau2->m_finalStateThreeProngs.size() > 0)
	{
		tau2ProngSize = 3;
	}
	else if (selectedTau2->m_finalStateFiveProngs.size() > 0)
	{
		tau2ProngSize = 5;
	}

	product.m_tau1DecayMode = tau1DecayMode;
	product.m_tau2DecayMode = tau2DecayMode;
	product.m_tau1ProngSize = tau1ProngSize;
	product.m_tau2ProngSize = tau2ProngSize;

	//ugly temporary solution
	for(typename std::vector<KGenTau>::const_iterator genTau = event.m_genTaus->begin();
	    genTau != event.m_genTaus->end(); ++genTau)
	{
		if (selectedTau1->m_node->p4 == genTau->p4)
		{
			product.m_genMatchedDecayMode[&(*genTau)] = tau1DecayMode;
			product.m_genMatchedProngSize[&(*genTau)] = tau1ProngSize;
		}
		if (selectedTau2->m_node->p4 == genTau->p4)
		{
			product.m_genMatchedDecayMode[&(*genTau)] = tau2DecayMode;
			product.m_genMatchedProngSize[&(*genTau)] = tau2ProngSize;
		}
	}


	std::map<GenParticleDecayTree::DecayMode, int> finalStateLeptons;

	finalStateLeptons[product.m_tau1DecayMode]++;
	finalStateLeptons[product.m_tau2DecayMode]++;

	std::map<GenParticleDecayTree::DecayMode, int>::iterator tau1 = finalStateLeptons.begin();
	if (tau1->second == 2)
	{
		if (tau1->first == GenParticleDecayTree::DecayMode::E)
		{
			product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::EE;
		}
		else if (tau1->first == GenParticleDecayTree::DecayMode::M)
		{
			product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::MM;
		}
		else
		{
			product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::TT;
		}
	}
	else
	{
		std::map<GenParticleDecayTree::DecayMode, int>::iterator tau2 = finalStateLeptons.begin();
		++tau2;
		if (tau1->first == GenParticleDecayTree::DecayMode::E)
		{
			if (tau2->first == GenParticleDecayTree::DecayMode::M)
			{
				product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::EM;
			}
			else
			{
				product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::ET;
			}
		}
		else if (tau1->first == GenParticleDecayTree::DecayMode::M)
		{
			product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::MT;
		}
		else
		{
			product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::TT;
		}
	}
}
