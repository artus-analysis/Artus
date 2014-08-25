#pragma once

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/ProducerBase.h"


/**
   \brief CrossSectionWeightProducer
   Config tags:
   - Xsection, e.g. "1234"
   If config tag available, provided value is used as weight.
   If not available, external or internal cross section of the
   LumiSection is used.

*/


template<class TTypes>
class CrossSectionWeightProducer : public ProducerBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;

	virtual std::string GetProducerId() const {
		return "cross_section_weight";
	}

	virtual void Produce(event_type const& event,
			product_type & product,
			setting_type const& settings) const {
		if (settings.GetXSection() > 0.)
			product.m_weights["crossSectionPerEventWeight"] = settings.GetXSection();
		else if (event.m_genLumiMetadata->xSectionExt > 0.)
			product.m_weights["crossSectionPerEventWeight"] = event.m_genLumiMetadata->xSectionExt;
		else if (event.m_genLumiMetadata->xSectionInt > 0.)
			product.m_weights["crossSectionPerEventWeight"] = event.m_genLumiMetadata->xSectionInt;
		else
			LOG(ERROR) << "No CrossSection information found.";

	}

};
