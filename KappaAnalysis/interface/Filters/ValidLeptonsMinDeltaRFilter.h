
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
class ValidLeptonsMinDeltaRFilterBase: public FilterBase<KappaTypes> {
public:

	typedef typename KappaTypes::event_type event_type;
	typedef typename KappaTypes::product_type product_type;
	typedef typename KappaTypes::setting_type setting_type;

	ValidLeptonsMinDeltaRFilterBase(std::vector<TValidObject*> product_type::*validLeptons,
	                      float (setting_type::*GetMinDeltaRValidLeptons)(void) const) :
		m_validLeptons(validLeptons),
		GetMinDeltaRValidLeptons(GetMinDeltaRValidLeptons)
	{
	}

	virtual bool DoesEventPass(event_type const& event, product_type const& product,
	                           setting_type const& settings) const ARTUS_CPP11_OVERRIDE
	{
		if ((product.*m_validLeptons).size() >= 2)
		{
			float deltaRMatched = 0;
			//int matchid1 = 0;
			for (typename std::vector<TValidObject*>::const_iterator validObject1 = (product.*m_validLeptons).begin();
			validObject1 != (product.*m_validLeptons).end(); ++validObject1)
			{
				//int matchid2 = 0;
				for (typename std::vector<TValidObject*>::const_iterator validObject2 = (product.*m_validLeptons).begin();
						validObject2 != (product.*m_validLeptons).end(); ++validObject2)
				{
					//make sure not to match lepton with itself
					if (validObject1 != validObject2)
					{
						deltaRMatched = ROOT::Math::VectorUtil::DeltaR((*validObject1)->p4, (*validObject2)->p4);
						if (deltaRMatched < (settings.*GetMinDeltaRValidLeptons)())
						{
							//std::cout << "Deleting event with DeltaR between particles of " << deltaRMatched << std::endl;
							return false;
						}
					}
					//matchid2 += 1;
				}
				//matchid1 += 1;
			}
		}
		return true;
	};

private:
	std::vector<TValidObject*> KappaProduct::*m_validLeptons;
	float (setting_type::*GetMinDeltaRValidLeptons)(void) const;
};

/** Filter events with too close valid electrons
 */
class ValidElectronsMinDeltaRFilter: public ValidLeptonsMinDeltaRFilterBase<KElectron>
{

public:

	ValidElectronsMinDeltaRFilter();
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE;
};


/** Filter events with too close valid muons
 */
class ValidMuonsMinDeltaRFilter: public ValidLeptonsMinDeltaRFilterBase<KMuon>
{

public:

	ValidMuonsMinDeltaRFilter();
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE;
};


/** Filter events with too close valid taus
 */
class ValidTausMinDeltaRFilter: public ValidLeptonsMinDeltaRFilterBase<KTau>
{

public:

	ValidTausMinDeltaRFilter();
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE;
};

/** Filter events with too close valid leptons
 */
class ValidLeptonsMinDeltaRFilter: public ValidLeptonsMinDeltaRFilterBase<KLepton>
{

public:

	ValidLeptonsMinDeltaRFilter();
	virtual std::string GetFilterId() const ARTUS_CPP11_OVERRIDE;
};

