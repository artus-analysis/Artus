
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
}

void GenTauDecayModeProducer::Produce(KappaEvent const& event, KappaProduct& product,
                                      KappaSettings const& settings) const
{
	assert(event.m_genTaus);

	MotherDaughterBundle* selectedTau1 = nullptr;
	MotherDaughterBundle* selectedTau2 = nullptr;

	int tau1ProngSize = -1;
	int tau2ProngSize = -1;

	MotherDaughterBundle::DecayMode tau1DecayMode = MotherDaughterBundle::DecayMode::NONE;
	MotherDaughterBundle::DecayMode tau2DecayMode = MotherDaughterBundle::DecayMode::NONE;

	if ((product.m_genBoson.size() > 0) && (product.m_genBoson[0].Daughters.size() > 1))
	{
		selectedTau1 = &(product.m_genBoson[0].Daughters[0]);
		selectedTau2 = &(product.m_genBoson[0].Daughters[1]);

		selectedTau1->determineDecayMode(selectedTau1);
		selectedTau2->determineDecayMode(selectedTau2);

		tau1DecayMode = selectedTau1->decayMode;
		tau2DecayMode = selectedTau2->decayMode;

		selectedTau1->createFinalStateProngs(selectedTau1);
		selectedTau2->createFinalStateProngs(selectedTau2);

		if ((selectedTau1->finalStateOneProngs.size()) > 0)
		{
			tau1ProngSize = 1;
		}
		else if ((selectedTau1->finalStateThreeProngs.size()) > 0)
		{
			tau1ProngSize = 3;
		}
		else if ((selectedTau1->finalStateFiveProngs.size()) > 0)
		{
			tau1ProngSize = 5;
		}

		if (selectedTau2->finalStateOneProngs.size() > 0)
		{
			tau2ProngSize = 1;
		}
		else if (selectedTau2->finalStateThreeProngs.size() > 0)
		{
			tau2ProngSize = 3;
		}
		else if (selectedTau2->finalStateFiveProngs.size() > 0)
		{
			tau2ProngSize = 5;
		}
	}

	product.m_tau1DecayMode = tau1DecayMode;
	product.m_tau2DecayMode = tau2DecayMode;
	product.m_tau1ProngSize = tau1ProngSize;
	product.m_tau2ProngSize = tau2ProngSize;

	//ugly temporary solution
	for(typename std::vector<KGenTau>::const_iterator genTau = event.m_genTaus->begin();
	    genTau != event.m_genTaus->end(); ++genTau)
	{
		if (selectedTau1->node->p4 == genTau->p4)
		{
			product.m_genMatchedDecayMode[&(*genTau)] = tau1DecayMode;
			product.m_genMatchedProngSize[&(*genTau)] = tau1ProngSize;
		}
		if (selectedTau2->node->p4 == genTau->p4)
		{
			product.m_genMatchedDecayMode[&(*genTau)] = tau2DecayMode;
			product.m_genMatchedProngSize[&(*genTau)] = tau2ProngSize;
		}
	}


	std::map<MotherDaughterBundle::DecayMode, int> finalStateLeptons;

	finalStateLeptons[product.m_tau1DecayMode]++;
	finalStateLeptons[product.m_tau2DecayMode]++;

	std::map<MotherDaughterBundle::DecayMode, int>::iterator tau1 = finalStateLeptons.begin();
	if (tau1->second == 2)
	{
		if (tau1->first == MotherDaughterBundle::DecayMode::E)
		{
			product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::EE;
		}
		else if (tau1->first == MotherDaughterBundle::DecayMode::M)
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
		std::map<MotherDaughterBundle::DecayMode, int>::iterator tau2 = finalStateLeptons.begin();
		++tau2;
		if (tau1->first == MotherDaughterBundle::DecayMode::E)
		{
			if (tau2->first == MotherDaughterBundle::DecayMode::M)
			{
				product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::EM;
			}
			else
			{
				product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::ET;
			}
		}
		else if (tau1->first == MotherDaughterBundle::DecayMode::M)
		{
			product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::MT;
		}
		else
		{
			product.m_genTauTauDecayMode = KappaEnumTypes::TauTauDecayMode::TT;
		}
	}
}
