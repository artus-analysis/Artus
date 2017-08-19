
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"

#include "Artus/Core/interface/FilterBase.h"


/** Abstract filter class that filters events where valid objects match a given trigger
 *
 *	Needs to run after the trigger matching producers.
 */
template<class TValidObject>
class TriggerMatchingFilterBase: public FilterBase<KappaTypes>
{

public:

	
	TriggerMatchingFilterBase(std::map<TValidObject*, KLV*> product_type::*triggerMatchedObjects,
	                          std::vector<TValidObject*> product_type::*validObjects,
	                          size_t (setting_type::*GetMinNMatchedObjects)(void) const) :
		m_triggerMatchedObjects(triggerMatchedObjects),
		m_validObjects(validObjects),
		GetMinNMatchedObjects(GetMinNMatchedObjects)
	{
	}

	bool DoesEventPass(event_type const& event, product_type const& product,
	                   setting_type const& settings, metadata_type const& metadata) const override
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
	std::map<TValidObject*, KLV*> product_type::*m_triggerMatchedObjects;
	std::vector<TValidObject*> product_type::*m_validObjects;
	size_t (setting_type::*GetMinNMatchedObjects)(void) const;

};


/** Filter trigger matched electrons
 *  Required config tags:
 *  - MinNMatchedElectrons (default 0)
 */
class ElectronTriggerMatchingFilter: public TriggerMatchingFilterBase<KElectron>
{

public:

	ElectronTriggerMatchingFilter();
	std::string GetFilterId() const override;
};


/** Filter trigger matched muons
 *  Required config tags:
 *  - MinNMatchedMuons (default 0)
 */
class MuonTriggerMatchingFilter: public TriggerMatchingFilterBase<KMuon>
{

public:

	MuonTriggerMatchingFilter();
	std::string GetFilterId() const override;

};


/** Filter trigger matched taus
 *  Required config tags:
 *  - MinNMatchedTaus (default 0)
 */
class TauTriggerMatchingFilter: public TriggerMatchingFilterBase<KTau>
{

public:

	TauTriggerMatchingFilter();
	std::string GetFilterId() const override;

};


/** Filter trigger matched jets
 *  Required config tags:
 *  - MinNMatchedJets (default 0)
 */
class JetTriggerMatchingFilter: public TriggerMatchingFilterBase<KBasicJet>
{

public:

	JetTriggerMatchingFilter();
	std::string GetFilterId() const override;

};

