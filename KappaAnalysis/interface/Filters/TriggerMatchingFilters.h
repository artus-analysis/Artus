
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/FilterBase.h"


/** Abstract filter class that filters events where valid objects match a given trigger
 *
 *	Needs to run after the trigger matching producers.
 */
template<class TTypes, class TValidObject>
class TriggerMatchingFilterBase: public FilterBase<TTypes>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	TriggerMatchingFilterBase(std::map<TValidObject*, KDataLV*> product_type::*triggerMatchedObjects,
	                          std::vector<TValidObject*> product_type::*validObjects,
	                          size_t (setting_type::*GetMinNMatchedObjects)(void) const) :
		m_triggerMatchedObjects(triggerMatchedObjects),
		m_validObjects(validObjects),
		GetMinNMatchedObjects(GetMinNMatchedObjects)
	{
	}

	virtual bool DoesEventPass(event_type const& event, product_type const& product,
	                           setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		if (((product.*m_triggerMatchedObjects).size() < (product.*m_validObjects).size()) ||
		    ((product.*m_triggerMatchedObjects).size() < (settings.*GetMinNMatchedObjects)()))
		{
			return false;
		}
		else
		{
			return true;
		}
	}


private:
	std::map<TValidObject*, KDataLV*> product_type::*m_triggerMatchedObjects;
	std::vector<TValidObject*> product_type::*m_validObjects;
	size_t (setting_type::*GetMinNMatchedObjects)(void) const;

};


/** Filter trigger matched electrons
 *  Required config tags:
 *  - MinNMatchedElectrons (default 0)
 */
template<class TTypes>
class ElectronTriggerMatchingFilter: public TriggerMatchingFilterBase<TTypes, KDataElectron>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "electron_trigger_matching";
	}
	
	ElectronTriggerMatchingFilter() :
		TriggerMatchingFilterBase<TTypes, KDataElectron>(&product_type::m_triggerMatchedElectrons,
		                                                 &product_type::m_validElectrons,
		                                                 &setting_type::GetMinNMatchedElectrons)
	{
	}

};


/** Filter trigger matched muons
 *  Required config tags:
 *  - MinNMatchedMuons (default 0)
 */
template<class TTypes>
class MuonTriggerMatchingFilter: public TriggerMatchingFilterBase<TTypes, KDataMuon>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "muon_trigger_matching";
	}
	
	MuonTriggerMatchingFilter() :
		TriggerMatchingFilterBase<TTypes, KDataMuon>(&product_type::m_triggerMatchedMuons,
		                                             &product_type::m_validMuons,
		                                             &setting_type::GetMinNMatchedMuons)
	{
	}

};


/** Filter trigger matched taus
 *  Required config tags:
 *  - MinNMatchedTaus (default 0)
 */
template<class TTypes>
class TauTriggerMatchingFilter: public TriggerMatchingFilterBase<TTypes, KDataPFTau>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "tau_trigger_matching";
	}
	
	TauTriggerMatchingFilter() :
		TriggerMatchingFilterBase<TTypes, KDataPFTau>(&product_type::m_triggerMatchedTaus,
		                                              &product_type::m_validTaus,
		                                              &setting_type::GetMinNMatchedTaus)
	{
	}

};


/** Filter trigger matched jets
 *  Required config tags:
 *  - MinNMatchedJets (default 0)
 */
template<class TTypes>
class JetTriggerMatchingFilter: public TriggerMatchingFilterBase<TTypes, KDataPFJet>
{

public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "jet_trigger_matching";
	}
	
	JetTriggerMatchingFilter() :
		TriggerMatchingFilterBase<TTypes, KDataPFJet>(&product_type::m_triggerMatchedJets,
		                                              &product_type::m_validJets,
		                                              &setting_type::GetMinNMatchedJets)
	{
	}

};

