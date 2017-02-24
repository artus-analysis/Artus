
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Core/interface/FilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"


/** Abstract filter class that filters events where valid objects match a given generator Tauon
 *
 *	Needs to run after the gen matching producers and valid objects producer
 */
template<class TValidObject>
class GenMatchingFilterBase: public FilterBase<KappaTypes>
{

public:
	
	GenMatchingFilterBase(std::map<TValidObject*, KGenParticle*> KappaProduct::*genParticleMatchedObjects,
	                      std::vector<TValidObject*> KappaProduct::*validObjects) :
		m_genParticleMatchedObjects(genParticleMatchedObjects),
		m_validObjects(validObjects)
	{
	}

	bool DoesEventPass(KappaEvent const& event, KappaProduct const& product,
	                           KappaSettings const& settings) const override
	{
		if ((product.*m_genParticleMatchedObjects).size() == 0) 
		{
			return false;
		}
		else
		{
			return true;
		}
	}


private:
	std::map<TValidObject*, KGenParticle*> KappaProduct::*m_genParticleMatchedObjects;
	std::vector<TValidObject*> KappaProduct::*m_validObjects;

};


/** Filter gen matched electrons
 *  Required config tags:
 *  - MinNMatchedElectrons (default 0)
 */
class ElectronGenMatchingFilter: public GenMatchingFilterBase<KElectron>
{

public:
	ElectronGenMatchingFilter();

};


/** Filter gen matched muons
 *  Required config tags:
 *  - MinNMatchedMuons (default 0)
 */
class MuonGenMatchingFilter: public GenMatchingFilterBase<KMuon>
{

public:
	MuonGenMatchingFilter();

};


/** Filter gen matched taus
 *  Required config tags:
 *  - MinNMatchedTaus (default 0)
 */
class TauGenMatchingFilter: public GenMatchingFilterBase<KTau>
{

public:
	TauGenMatchingFilter();

};


/** Filter gen matched jets
 *  Required config tags:
 *  - MinNMatchedJets (default 0)
 */
class JetGenMatchingFilter: public GenMatchingFilterBase<KBasicJet>
{

public:
	JetGenMatchingFilter();

};

