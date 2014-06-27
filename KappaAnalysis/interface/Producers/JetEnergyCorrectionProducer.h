
#pragma once

#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>

#include <Math/VectorUtil.h>

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/ProducerBase.h"
#include "Artus/Utility/interface/Utility.h"

/**
   \brief Producer for 
*/


template<class TTypes, class TJet, class TValidJet>
class JetEnergyCorrectionProducerBase: public ProducerBase<TTypes>, public ValidPhysicsObjectTools<TTypes, TValidJet>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	JetEnergyCorrectionProducerBase(std::vector<TJet>* event_type::*jets, std::vector<TValidJet*> product_type::*validJets) :
		ProducerBase<TTypes>(),
		ValidPhysicsObjectTools<TTypes, TValidJet>(validJets),
		m_jetsMember(jets)
	{
	}

	virtual void Init(setting_type const& settings) ARTUS_CPP11_OVERRIDE
	{
		ProducerBase<TTypes>::Init(settings);
	}

	virtual void Produce(event_type const& event, product_type& product,
	                     setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
	}


private:
	std::vector<TJet>* event_type::*m_jetsMember;

};



/**
   \brief Producer for 
   
   Operates on the vector event.m_jets.
*/
template<class TTypes>
class JetEnergyCorrectionProducer: public JetEnergyCorrectionProducerBase<TTypes, KDataPFJet, KDataPFJet>
{
public:
	JetEnergyCorrectionProducer() :
		JetEnergyCorrectionProducerBase<TTypes, KDataPFJet, KDataPFJet>(&TTypes::event_type::m_jets,
	                                                                    &TTypes::product_type::m_validJets)
	{
	};
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "jet_energy_correction";
	}
};



/**
   \brief Producer for 
   
   Operates on the vector event.m_tjets.
*/
template<class TTypes>
class TaggedJetEnergyCorrectionProducer: public JetEnergyCorrectionProducerBase<TTypes, KDataPFTaggedJet, KDataPFJet>
{
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	TaggedJetEnergyCorrectionProducer() :
		JetEnergyCorrectionProducerBase<TTypes, KDataPFTaggedJet, KDataPFJet>(&TTypes::event_type::m_tjets,
		                                                                      &TTypes::product_type::m_validJets)
	{
	};
	
	virtual std::string GetProducerId() const ARTUS_CPP11_OVERRIDE {
		return "tagged_jet_energy_correction";
	}
};


