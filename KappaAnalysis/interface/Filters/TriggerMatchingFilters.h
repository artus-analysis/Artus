
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

	
	TriggerMatchingFilterBase(std::map<TValidObject*, KLV*> KappaProduct::*triggerMatchedObjects,
	                          std::vector<TValidObject*> KappaProduct::*validObjects,
	                          size_t (KappaSettings::*GetMinNMatchedObjects)(void) const) :
		m_triggerMatchedObjects(triggerMatchedObjects),
		m_validObjects(validObjects),
		GetMinNMatchedObjects(GetMinNMatchedObjects)
	{
	}

	bool DoesEventPass(KappaEvent const& event, KappaProduct const& product,
	                           KappaSettings const& settings) const override
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
	std::map<TValidObject*, KLV*> KappaProduct::*m_triggerMatchedObjects;
	std::vector<TValidObject*> KappaProduct::*m_validObjects;
	size_t (KappaSettings::*GetMinNMatchedObjects)(void) const;

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

