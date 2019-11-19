
#pragma once

#include "Artus/Core/interface/FilterBase.h"
#include "Artus/Utility/interface/CutRange.h"


/** Base class for simple cuts (upper/lower thresholds, ranges, equal comparisons)
	In order to uses this abstract base class, you need to overwrite the Init...
	functions and fill the vector m_cuts.
 */
template<class TTypes>
class CutRangeFilterBase: public FilterBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	typedef typename TTypes::metadata_type metadata_type;

	typedef typename std::function<double(event_type const&, product_type const&, setting_type const&, metadata_type const&)> double_extractor_lambda;

	void Init(setting_type const& settings, metadata_type& metadata)  override
	{
		FilterBase<TTypes>::Init(settings, metadata);
	}

	virtual bool DoesEventPass(event_type const& event, product_type const& product,
			setting_type const& settings, metadata_type const& metadata) const
	{
		{
			bool passAllCuts = true;

			for (auto cut : m_cuts)
			{
				passAllCuts = passAllCuts && cut.second.IsInRange(cut.first(event, product, settings, metadata));
				if (! passAllCuts) {
					break;
				}
			}
			return passAllCuts;
		}
	}

protected:
	std::vector<std::pair<double_extractor_lambda, CutRange> > m_cuts;

};
