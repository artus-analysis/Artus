
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/FilterBase.h"


/** Abstract filter class that filters events where valid objects match a given generator Tauon
 *
 *	Needs to run after the gen matching producers and valid objects producer
 */
template<class TValidObject>
class GenMatchingFilterBase: public FilterBase<KappaTypes>
{

public:
	
	GenMatchingFilterBase(std::map<TValidObject*, KGenParticle*> KappaProduct::*genMatchedObjects,
	                          std::vector<TValidObject*> KappaProduct::*validObjects) :
		m_genMatchedObjects(genMatchedObjects),
		m_validObjects(validObjects)
	{
	}

	virtual bool DoesEventPass(KappaEvent const& event, KappaProduct const& product,
	                           KappaSettings const& settings) const ARTUS_CPP11_OVERRIDE
	{
		if ((product.*m_genMatchedObjects).size() == 0) 
		{
			return false;
		}
		else
		{
			return true;
		}
	}


private:
	std::map<TValidObject*, KGenParticle*> KappaProduct::*m_genMatchedObjects;
	std::vector<TValidObject*> KappaProduct::*m_validObjects;

};


/** Filter gen matched electrons
 *  Required config tags:
 *  - MinNMatchedElectrons (default 0)
 */
class ElectronGenMatchingFilter: public GenMatchingFilterBase<KDataElectron>
{

public:

	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "ElectronGenMatchingFilter";
	}
	
	ElectronGenMatchingFilter() :
		GenMatchingFilterBase<KDataElectron>(&KappaProduct::m_genMatchedElectrons,
		                                     &KappaProduct::m_validElectrons)
	{
	}

};


/** Filter gen matched muons
 *  Required config tags:
 *  - MinNMatchedMuons (default 0)
 */
class MuonGenMatchingFilter: public GenMatchingFilterBase<KDataMuon>
{

public:

	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "MuonGenMatchingFilter";
	}
	
	MuonGenMatchingFilter() :
		GenMatchingFilterBase<KDataMuon>(&KappaProduct::m_genMatchedMuons,
		                                 &KappaProduct::m_validMuons)
	{
	}

};


/** Filter gen matched taus
 *  Required config tags:
 *  - MinNMatchedTaus (default 0)
 */
class TauGenMatchingFilter: public GenMatchingFilterBase<KDataPFTau>
{

public:

	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "TauGenMatchingFilter";
	}
	
	TauGenMatchingFilter() :
		GenMatchingFilterBase<KDataPFTau>(&KappaProduct::m_genMatchedTaus,
		                                  &KappaProduct::m_validTaus)
	{
	}

};


/** Filter gen matched jets
 *  Required config tags:
 *  - MinNMatchedJets (default 0)
 */
class JetGenMatchingFilter: public GenMatchingFilterBase<KDataPFJet>
{

public:

	
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE {
		return "JetGenMatchingFilter";
	}
	
	JetGenMatchingFilter() :
		GenMatchingFilterBase<KDataPFJet>(&KappaProduct::m_genMatchedJets,
		                                  &KappaProduct::m_validJets)
	{
	}

};

