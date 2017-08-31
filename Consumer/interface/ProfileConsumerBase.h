
#pragma once

#include <boost/scoped_ptr.hpp>

#include "DrawConsumerBase.h"
#include "Profile2D.h"

template<class TTypes>
class ProfileConsumerBase: public ConsumerBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	typedef typename TTypes::metadata_type metadata_type;

	typedef std::function<
			std::vector<float>(event_type const&, product_type const& )>
					            ValueExtractLambda;
	typedef std::pair<ValueExtractLambda, ValueModifiers> ValueDesc;

	typedef Pipeline<TTypes> PipelineTypeForThis;

	ProfileConsumerBase(std::string plotName, ValueDesc xsource,
			ValueDesc ysource) :
			m_plotName(plotName), m_xsource(xsource), m_ysource(ysource) {
	}

	std::string GetConsumerId() const override {
		return "profile";
	}

	void Init(setting_type const& settings, metadata_type& metadata) override
	{
		ConsumerBase<TTypes>::Init(settings, metadata);

		m_profile.reset(
				new Profile2d(m_plotName,
						settings.GetRootFileFolder()));

		for (auto const& m : m_xsource.second) {
			m->applyProfileBeforeCreation(m_profile.get(), 0);
		}
		for (auto const& m : m_ysource.second) {
			m->applyProfileBeforeCreation(m_profile.get(), 1);
		}

		m_profile->Init();
	}

	void ProcessEvent(event_type const& event, product_type const& product,
			setting_type const& settings, metadata_type const& metadata,
			FilterResult& result) override {
		ConsumerBase<TTypes>::ProcessEvent(event, product, settings, metadata, result);

		// not supported ..
	}

	void ProcessFilteredEvent(event_type const& event, product_type const& product,
			setting_type const& settings, metadata_type const& metadata) override
	{
		ConsumerBase<TTypes>::ProcessFilteredEvent(event, product, settings, metadata);

		auto resX = m_xsource.first(event, product);
		auto resY = m_ysource.first(event, product);

		if ((resX.size() == 0) || (resY.size() == 0))
			return;

		size_t ix = 0;
		size_t iy = 0;
		bool terminate = false;
		while (!terminate) {

			m_profile->AddPoint(resX[ix], resY[iy], 1.0f);

			bool oneIncreased = false;
			if (ix < (resX.size() - 1)) {
				++ix;
				oneIncreased = true;
			}
			if (iy < (resY.size() - 1)) {
				++iy;
				oneIncreased = true;
			}
			if (!oneIncreased)
				break;
		}
	}

	void Finish(setting_type const& settings, metadata_type const& metadata) override {
		m_profile->Store(settings.GetRootOutFile());
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

