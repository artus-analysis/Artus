
#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"

/**
   \brief Producer for valid jets.
   
   Valid jets pass the particle flow JetID ("tight", "medium" or "loose") by JetMET. There are no
   pileupJetID requirements applied at the moment.

   This producer requires the "JetID" config tag to be set to "tight", "medium" or "loose".
   
   This is a templated base version. Use the actual versions for KDataPFJets or KDataPFTaggedJets
   at the end of this file.
*/


template<class TTypes, class TJet>
class ValidJetsProducerBase: public ProducerBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::global_setting_type global_setting_type;
	typedef typename TTypes::setting_type setting_type;
	
	ValidJetsProducerBase(std::vector<TJet>* event_type::*jets) : m_jetsMember(jets) {
	}

	virtual void InitGlobal(global_setting_type const& globalSettings)
	{
		GetMaxNeutralFraction(globalSettings.GetJetID());
	}

	virtual void InitLocal(setting_type const& pipelineSettings)
	{
		GetMaxNeutralFraction(pipelineSettings.GetJetID());
	}

	virtual void ProduceGlobal(event_type const& event,
	                           product_type& product,
	                           global_setting_type const& globalSettings) const ARTUS_CPP11_OVERRIDE
	{
		Produce(event, product);
	}

	virtual void ProduceLocal(event_type const& event,
	                          product_type& product,
	                          setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		Produce(event, product);
	}

private:
	std::vector<TJet>* event_type::*m_jetsMember;
	float max_neutral_fraction;

	void GetMaxNeutralFraction(std::string jetid)
	{
		std::string tmpjetid = boost::algorithm::to_lower_copy(boost::algorithm::trim_copy(jetid));
		if (tmpjetid == "tight")
			max_neutral_fraction = 0.90;
		else if (tmpjetid == "medium")
			max_neutral_fraction = 0.95;
		else if (tmpjetid == "loose")
			max_neutral_fraction = 0.99;
		else
			LOG(FATAL) << "Jet ID " << jetid << " not implemented!";
	}

	// function that lets this producer work as both a global and a local producer
	virtual void Produce(event_type const& event, product_type& product) const
	{
		for (typename std::vector<TJet>::iterator jet = (event.*m_jetsMember)->begin();
		     jet != (event.*m_jetsMember)->end(); ++jet)
		{
			bool validJet = true;

			// JetID
			// https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
			// jets, all eta
			validJet = validJet
					   && jet->neutralHadFraction + jet->HFHadFraction < max_neutral_fraction
					   && jet->neutralEMFraction < max_neutral_fraction
					   && jet->nConst > 1;
			// jets, |eta| < 2.4 (tracker)
			if (std::abs(jet->p4.eta()) < 2.4)
			{
				validJet = validJet
						   && jet->chargedHadFraction > 0.0
						   && jet->nCharged > 0
						   && jet->chargedEMFraction < 0.99;
			}

			/* TODO
			if (globalSettings.Global()->GetVetoPileupJets())
			{
				bool puID = static_cast<KDataPFTaggedJet*>jet->getpuJetID("PUJetIDFullMedium", event.m_taggermetadata);
				validJet = validJet && puID;
			}
			*/
			
			// check possible analysis-specific criteria
			validJet = validJet && AdditionalCriteria(&(*jet), event, product);

			if (validJet)
				product.m_validJets.push_back(&(*jet));
			else
				product.m_invalidJets.push_back(&(*jet));
		}
	}
	
	// Can be overwritten for analysis-specific use cases
	virtual bool AdditionalCriteria(TJet* jet, event_type const& event, product_type& product) const
	{
		bool validJet = true;
		
		return validJet;
	}

};



/**
   \brief Producer for valid jets (simple PF jets).
   
   Operates on the vector event.m_jets.
*/
template<class TTypes>
class ValidJetsProducer: public ValidJetsProducerBase<TTypes, KDataPFJet>
{
public:
	ValidJetsProducer() : ValidJetsProducerBase<TTypes, KDataPFJet>(&TTypes::event_type::m_jets) {};
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "valid_jets";
	}
};



/**
   \brief Producer for valid jets (tagged PF jets).
   
   Operates on the vector event.m_tjets.
*/
template<class TTypes>
class ValidTaggedJetsProducer: public ValidJetsProducerBase<TTypes, KDataPFTaggedJet>
{
public:
	ValidTaggedJetsProducer() : ValidJetsProducerBase<TTypes, KDataPFTaggedJet>(&TTypes::event_type::m_tjets) {};
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "valid_tagged_jets";
	}
};


