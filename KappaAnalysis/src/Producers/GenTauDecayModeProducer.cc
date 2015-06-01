
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
	
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity( "genTau1DecayMode",[](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_tau1DecayMode;
	} );
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity( "genTau2DecayMode",[](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_tau2DecayMode;
	} );
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity( "genTau1ProngSize",[](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_tau1ProngSize;
	} );
	LambdaNtupleConsumer<KappaTypes>::AddIntQuantity( "genTau2ProngSize",[](KappaEvent const & event, KappaProduct const & product)
	{
		return product.m_tau2ProngSize;
	} );
}

void GenTauDecayModeProducer::Produce(KappaEvent const& event, KappaProduct& product,
                     KappaSettings const& settings) const
{
	assert(event.m_genTaus);

	MotherDaughterBundle* selectedTau1 = 0;
	MotherDaughterBundle* selectedTau2 = 0;

	int tau1ProngSize = -1;
	int tau2ProngSize = -1;

	int tau1DecayMode = -1;
	int tau2DecayMode = -1;


	if(product.m_genBoson.size()>0)
	{
		if(product.m_genBoson[0].Daughters.size()>1)
		{
			selectedTau1 = &(product.m_genBoson[0].Daughters[0]);
			selectedTau2 = &(product.m_genBoson[0].Daughters[1]);
			
			selectedTau1->determineDecayMode(selectedTau1);
			selectedTau2->determineDecayMode(selectedTau2);

			tau1DecayMode = Utility::ToUnderlyingValue(selectedTau1->decayMode);
			tau2DecayMode = Utility::ToUnderlyingValue(selectedTau2->decayMode);

			selectedTau1->createFinalStateProngs(selectedTau1);
			selectedTau2->createFinalStateProngs(selectedTau2);

			if ((selectedTau1->finalStateOneProngs.size())>0) tau1ProngSize = 1;			
			else if ((selectedTau1->finalStateThreeProngs.size())>0) tau1ProngSize = 3;
			else if ((selectedTau1->finalStateFiveProngs.size())>0) tau1ProngSize = 5;
			
			if (selectedTau2->finalStateOneProngs.size()>0) tau2ProngSize = 1;
			else if (selectedTau2->finalStateThreeProngs.size()>0) tau2ProngSize = 3;
			else if (selectedTau2->finalStateFiveProngs.size()>0) tau2ProngSize = 5;
		}
	}

	product.m_tau1DecayMode = tau1DecayMode;
	product.m_tau2DecayMode = tau2DecayMode;	

	product.m_tau1ProngSize = tau1ProngSize;
	product.m_tau2ProngSize = tau2ProngSize;

	//ugly temporary solution
	for(typename std::vector<KGenTau>::const_iterator genTau = event.m_genTaus->begin();
	                                                      genTau != event.m_genTaus->end();++genTau)
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


	std::map<int, int> finalStateLeptons;

	finalStateLeptons[product.m_tau1DecayMode]++;
	finalStateLeptons[product.m_tau2DecayMode]++;

	std::map<int, int>::iterator tau1 = finalStateLeptons.begin();
	if (tau1->second == 2)
	{
		if(tau1->first == (int) MotherDaughterBundle::DecayMode::E)
			product.m_genTauDecayMode = (int) GenTauDecayMode::EE;
		else if(tau1->first == (int) MotherDaughterBundle::DecayMode::M)
			product.m_genTauDecayMode = (int) GenTauDecayMode::MM;
		else
			product.m_genTauDecayMode = (int) GenTauDecayMode::TT;
	}
	else
	{
		std::map<int, int>::iterator tau2 = finalStateLeptons.begin();
		++tau2;
		if(tau1->first == (int) MotherDaughterBundle::DecayMode::E)
		{
			if(tau2->first == (int) MotherDaughterBundle::DecayMode::M)
				product.m_genTauDecayMode = (int) GenTauDecayMode::EM;
			else
				product.m_genTauDecayMode = (int) GenTauDecayMode::ET;
		}
		else if(tau1->first == (int) MotherDaughterBundle::DecayMode::M)
				product.m_genTauDecayMode = (int) GenTauDecayMode::MT;
		else
			product.m_genTauDecayMode = (int) GenTauDecayMode::TT;
	}
}
