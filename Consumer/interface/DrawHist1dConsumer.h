
#pragma once

#include "DrawConsumerBase.h"
#include "Hist1D.h"
/*
 typedef std::function<
 std::vector<float>(MassRecoEvent const&, MassRecoProduct const&)> ValueExtractLambda;
 typedef std::pair<ValueExtractLambda, ValueModifiers> ValueDesc;
 */
template<class TTypes>
class DrawHist1dConsumerBase: public DrawConsumerBase<TTypes> {
public:

	typedef typename TTypes::event_type event_type;
	typedef typename TTypes::product_type product_type;
	typedef typename TTypes::setting_type setting_type;
	typedef typename TTypes::metadata_type metadata_type;

	typedef std::function<
			std::vector<float>(event_type const&, product_type const&
                                )> ValueExtractLambda;
	typedef std::pair<ValueExtractLambda, ValueModifiers> ValueDesc;

	DrawHist1dConsumerBase(std::string const& histName, ValueDesc desc) :
			DrawConsumerBase<TTypes>(), m_histName(histName), m_desc(desc) {

	}

	virtual ~DrawHist1dConsumerBase() {
	}

	std::string GetConsumerId() const override {
		return "hist1d";
	}

	void Init(setting_type const& settings, metadata_type& metadata) override {
		DrawConsumerBase<TTypes>::Init(settings, metadata);

		// init called
		const std::string rootFolder = settings.GetRootFileFolder();
		Hist1D * hist = new Hist1D(m_histName, rootFolder, m_desc.second);
		setHist(hist);
		assert(m_hist);
		m_hist->Init();
	}

	void Finish(setting_type const& settings, metadata_type const& metadata ) override {
		// store hist
		// + modifiers
		//LOG(INFO) << "Storing Hist for " << this->GetProductName() << ".";
		m_hist->Store(settings.GetRootOutFile());
	}

	void ProcessFilteredEvent(event_type const& event, product_type const& product,
			setting_type const& settings, metadata_type const& metadata) override {

		DrawConsumerBase<TTypes>::ProcessFilteredEvent(event, product, settings, metadata);

		auto res = m_desc.first(event, product);

		for (auto const& v : res) {
			getHist()->Fill(v, 1.0f);
		}
	}

	Hist1D* getHist() {
		return m_hist;
	}

	void setHist(Hist1D * hh) {
		m_hist = hh;
	}
private:
	std::string m_histName;
	ValueDesc m_desc;
	// already configured histogramm
	Hist1D* m_hist;
};
