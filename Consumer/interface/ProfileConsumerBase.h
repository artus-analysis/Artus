/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/scoped_ptr.hpp>

#include "Artus/Core/interface/Cpp11Support.h"

#include "DrawConsumerBase.h"
#include "Profile2D.h"

template<class TTypes>
class ProfileConsumerBase: public EventConsumerBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::local_product_type local_product_type;
	typedef typename TTypes::global_product_type global_product_type;
	typedef typename TTypes::setting_type setting_type;

	typedef std::function<
			std::vector<float>(event_type const&, global_product_type const&,
					local_product_type const&)> ValueExtractLambda;
	typedef std::pair<ValueExtractLambda, ValueModifiers> ValueDesc;

	typedef EventPipeline<TTypes> PipelineTypeForThis;

	ProfileConsumerBase(std::string plotName, ValueDesc xsource,
			ValueDesc ysource) :
			m_plotName(plotName), m_xsource(xsource), m_ysource(ysource) {
	}

	virtual void Init(PipelineTypeForThis * pset) ARTUS_CPP11_OVERRIDE {
		EventConsumerBase<TTypes>::Init(pset);

		m_profile.reset(
				new Profile2d(m_plotName,
						this->GetPipelineSettings().GetRootFileFolder()));

		for (auto const& m : m_xsource.second) {
			m->applyProfileBeforeCreation(m_profile.get(), 0);
		}
		for (auto const& m : m_ysource.second) {
			m->applyProfileBeforeCreation(m_profile.get(), 1);
		}

		m_profile->Init();
	}

	virtual void ProcessEvent(typename TTypes::event_type const& event,
			typename TTypes::global_product_type const& globalProduct,
			typename TTypes::local_product_type const& localProduct,
			FilterResult& result) ARTUS_CPP11_OVERRIDE {
		EventConsumerBase<TTypes>::ProcessEvent(event, globalProduct, localProduct,
				result);

		// not supported ..
	}

	virtual void ProcessFilteredEvent(typename TTypes::event_type const& event,
			typename TTypes::global_product_type const& globalProduct,
			typename TTypes::local_product_type const& localProduct)
					ARTUS_CPP11_OVERRIDE {
		EventConsumerBase<TTypes>::ProcessFilteredEvent(event, globalProduct,
				localProduct);

		auto resX = m_xsource.first(event, globalProduct, localProduct);
		auto resY = m_ysource.first(event, globalProduct, localProduct);

		if ((resX.size() == 0) || (resY.size() == 0))
			return;
		//assert(resX.size() == resY.size());

		size_t ix = 0;
		size_t iy = 0;
		bool terminate = false;
		while (!terminate) {

			m_profile->AddPoint(resX[ix], resY[iy], 1.0f);

			bool oneIncreased = false;
			if (ix < (resX.size() - 1)) {
				ix++;
				oneIncreased = true;
			}
			if (iy < (resY.size() - 1)) {
				iy++;
				oneIncreased = true;
			}
			if (!oneIncreased)
				break;
		}
	}

	virtual void Finish() {
		m_profile->Store(this->GetPipelineSettings().GetRootOutFile());
	}

	void SetPlotName(std::string plotName) {
		m_plotName = plotName;
	}

private:
	std::string m_plotName;
	ValueDesc m_xsource;
	ValueDesc m_ysource;
	boost::scoped_ptr<Profile2d> m_profile;
};

