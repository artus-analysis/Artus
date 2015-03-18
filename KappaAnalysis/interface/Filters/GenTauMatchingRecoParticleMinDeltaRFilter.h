
#pragma once

#include "Kappa/DataFormats/interface/Kappa.h"

#include "Artus/Utility/interface/DefaultValues.h"
#include "Artus/Core/interface/FilterBase.h"
#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaEnumTypes.h"


/** Abstract filter class that filters events where valid matched same flavoured leptons are not a minimum distance apart
 *
 *	Needs to run after the gen tau matching producer
 */
template<class TValidObject>
class GenTauMatchingRecoParticleMinDeltaRFilterBase: public FilterBase<KappaTypes> {
public:

	typedef typename KappaTypes::event_type event_type;
	typedef typename KappaTypes::product_type product_type;
	typedef typename KappaTypes::setting_type setting_type;

	GenTauMatchingRecoParticleMinDeltaRFilterBase(std::map<TValidObject*, KGenTau*> product_type::*genTauMatchedObjects,
	                      float (setting_type::*GetMinDeltaRMatchedRecoObjects)(void) const) :
		m_genTauMatchedObjects(genTauMatchedObjects),
		GetMinDeltaRMatchedRecoObjects(GetMinDeltaRMatchedRecoObjects)
	{
	}

	virtual bool DoesEventPass(event_type const& event, product_type const& product,
	                           setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		if ((product.*m_genTauMatchedObjects).size() >= 2)
		{
			float deltaRMatched = 0;
			for (typename std::map<TValidObject*, KGenTau*>::const_iterator validMatchedObject1 = (product.*m_genTauMatchedObjects).begin();
			validMatchedObject1 != (product.*m_genTauMatchedObjects).end(); ++validMatchedObject1)
			{
				for (typename std::map<TValidObject*, KGenTau*>::const_iterator validMatchedObject2 = (product.*m_genTauMatchedObjects).begin();
						validMatchedObject2 != (product.*m_genTauMatchedObjects).end(); ++validMatchedObject2)
				{
					//make sure not to match lepton with itself
					if (validMatchedObject1 != validMatchedObject2)
					{
						deltaRMatched = ROOT::Math::VectorUtil::DeltaR(validMatchedObject1->first->p4, validMatchedObject2->first->p4);
						if (deltaRMatched < (settings.*GetMinDeltaRMatchedRecoObjects)())
						{
							//std::cout << "Deleting event with DeltaR between matched particles of " << deltaRMatched << std::endl;
							return false;
						}
					}
				}
			}
		}
		return true;
	};

private:
	std::map<TValidObject*, KGenTau*> KappaProduct::*m_genTauMatchedObjects;
	float (setting_type::*GetMinDeltaRMatchedRecoObjects)(void) const;
};

/** Filter too close gen matched electrons
 */
class GenTauMatchingRecoElectronMinDeltaRFilter: public GenTauMatchingRecoParticleMinDeltaRFilterBase<KElectron>
{

public:

	GenTauMatchingRecoElectronMinDeltaRFilter();
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE;
};


/** Filter too close gen matched muons
 */
class GenTauMatchingRecoMuonMinDeltaRFilter: public GenTauMatchingRecoParticleMinDeltaRFilterBase<KMuon>
{

public:

	GenTauMatchingRecoMuonMinDeltaRFilter();
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE;
};


/** Filter too close gen matched taus
 */
class GenTauMatchingRecoTauMinDeltaRFilter: public GenTauMatchingRecoParticleMinDeltaRFilterBase<KTau>
{

public:

	GenTauMatchingRecoTauMinDeltaRFilter();
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE;
};
